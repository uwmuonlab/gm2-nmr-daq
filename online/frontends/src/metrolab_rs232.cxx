/********************************************************************\

Name:   Metrolab RS232 client
Author: Peter Winter
Email:  winterp@anl.gov

About:  A simple frontend to communicate with the Metrolab via RS232 and 
      that is synchronized to the SyncTrigger class in shim_trigger using 
      a SyncClient. Boilerplate code is surrounded with @sync flags and 
      places needing user code are marked with @user.  
        
\********************************************************************/

// ---std includes -----------------------------------------------//
#include <sys/socket.h>
#include <string>
#include <iostream>
#include <sstream>
#include <stdio.h>
using namespace std;

//--- other includes -----------------------------------------------//          
#include "midas.h"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"


 //--- project includes ---------------------------------------------//          
#include "sync_client.hh"

 //--- globals ------------------------------------------------------//          

#define FRONTEND_NAME "metrolab_rs232"

extern "C" {

  // The frontend name (client name) as seen by other MIDAS clients             
  char *frontend_name = (char*)FRONTEND_NAME;

  // The frontend file name, don't change it.                                   
  char *frontend_file_name = (char*) __FILE__;

  // frontend_loop is called periodically if this variable is TRUE              
  BOOL frontend_call_loop = FALSE;

  // A frontend status page is displayed with this frequency in ms.             
  INT display_period = 1000;

  // maximum event size produced by this frontend                               
  INT max_event_size = 0x80000; // 80 kB                         

  // maximum event size for fragmented events (EQ_FRAGMENTED)                   
  INT max_event_size_frag = 0x800000;

  // buffer size to hold events                                                 
  INT event_buffer_size = 0x800000;

  // Function declarations                                                      
  INT frontend_init();
  INT frontend_exit();
  INT begin_of_run(INT run_number, char *error);
  INT end_of_run(INT run_number, char *error);
  INT pause_run(INT run_number, char *error);
  INT resume_run(INT run_number, char *error);

  INT frontend_loop();
  INT read_trigger_event(char *pevent, INT off);
  INT poll_event(INT source, INT count, BOOL test);
  INT interrupt_configure(INT cmd, INT source, PTYPE adr);

  // Equipment list                                                             

  EQUIPMENT equipment[] =
    {
      {FRONTEND_NAME,  // equipment name                     
       {11, 0,         // event ID, trigger mask
        "SYSTEM",      // event buffer (use to be SYSTEM)
        EQ_POLLED |
        EQ_EB,         // equipment type
        0,             // not used
        "MIDAS",       // format
        TRUE,          // enabled
        RO_RUNNING |   // read only when running
        RO_ODB,        // and update ODB
        10,            // poll for 10ms
        0,             // stop run after this event limit                      
        0,             // number of sub events
        0,             // don't log history
        "", "", "",
       },

       read_trigger_event,      // readout routine                              
      },

      {""}
    };

} //extern C                                                                    

RUNINFO runinfo;

typedef struct {
   char address[128];
} METROLAB_SETTINGS;
METROLAB_SETTINGS metrolab_settings;

#define METROLAB_SETTINGS_STR "\
Address = STRING : [128] /dev/ttyUSB0\n\
"

int SerialPort;

// @sync: begin boilerplate                                                     
daq::SyncClient *listener = nullptr;
// @sync: end boilderplate

//--- Frontend Init -------------------------------------------------//         
INT frontend_init()
{
  // @sync: begin boilerplate                                                   
  //DATA part                                                                   
  HNDLE hDB, hkey;
  INT status, tmp;
  char str[256], filename[256];
  int size;

  cm_get_experiment_database(&hDB, NULL);
  
  sprintf(str,"/Equipment/%s/Settings",FRONTEND_NAME);
  status = db_create_record(hDB, 0, str, METROLAB_SETTINGS_STR);
  if (status != DB_SUCCESS){
    printf("Could not create record %s\n", str);
    return FE_ERR_ODB;   
  }  
  
  if(db_find_key(hDB, 0, str, &hkey)==DB_SUCCESS){
      printf("Hallo\n");
      size = sizeof(METROLAB_SETTINGS);
      db_get_record(hDB, hkey, &metrolab_settings, &size, 0);
  }
  
  
  
  /*
  db_find_key(hDB, 0, "Params/config-dir", &hkey);

  if (hkey) {
    size = sizeof(str);
    db_get_data(hDB, hkey, str, &size, TID_STRING);
    if (str[strlen(str) - 1] != DIR_SEPARATOR) {
      strcat(str, DIR_SEPARATOR_STR);
    }
  }

  // Get the config for the synchronized trigger.                               
  db_find_key(hDB, 0, "Params/sync-trigger-address", &hkey);

  if (hkey) {
    size = sizeof(str);
    db_get_data(hDB, hkey, str, &size, TID_STRING);
  }

  string trigger_addr(str);

  db_find_key(hDB, 0, "Params/fast-trigger-port", &hkey);

  if (hkey) {
    size = sizeof(tmp);
    db_get_data(hDB, hkey, &tmp, &size, TID_INT);
  }
  
  int trigger_port(tmp);
  
  // not sure that metrolab needs to by synced.
  // listener = new daq::SyncClient(trigger_addr, trigger_port);

  // @sync: end boilderplate                                                  
  // Note that if no address is specifed the SyncClient operates                
  // on localhost automatically.        
  */

  return SUCCESS;
}

//--- Frontend Exit ------------------------------------------------//          
INT frontend_exit()
{
  // @sync: begin boilerplate                                                   
  if (listener != nullptr) {
    delete listener;
  }
  // @sync: end boilerplate                                                     

  return SUCCESS;
}

//--- Begin of Run --------------------------------------------------//         
INT begin_of_run(INT run_number, char *error)
{
  // @sync: begin boilerplate
  if (listener != nullptr) {
    listener->SetReady();
  }
  // @sync: end boilerplate       

  return SUCCESS;
}

//--- End of Run -----------------------------------------------------//        
INT end_of_run(INT run_number, char *error)
{
  // @sync: begin boilerplate                                                  
  if (listener != nullptr) {
    listener->UnsetReady();
  }
  // @sync: end boilerplate       

  return SUCCESS;
}

//--- Pause Run -----------------------------------------------------*/        
INT pause_run(INT run_number, char *error){
  return SUCCESS;
}

//--- Resume Run ----------------------------------------------------*/       
INT resume_run(INT run_number, char *error)
{
  return SUCCESS;
}

//--- Frontend Loop -------------------------------------------------*/         

INT frontend_loop()
{
  // If frontend_call_loop is true, this routine gets called when              
  // the frontend is idle or once between every event                           
  return SUCCESS;
}

//-------------------------------------------------------------------*/         

/********************************************************************\           
  Readout routines for different events  

\********************************************************************/

//--- Trigger event routines ----------------------------------------*/         

INT poll_event(INT source, INT count, BOOL test) {

  static unsigned int i;
  bool rc = false;
  int pollcount = 0;
  int pollmax = 100;

  // fake calibration                                                           
  if (test) {
    for (i = 0; i < count; i++) {
      usleep(10);
    }
    return 0;
  }

  // @sync: begin boilerplate                                                  
  
  if (listener != nullptr) {
    if (listener->HasTrigger()) {
      // User: Issue trigger here.
      // Note that HasTrigger only returns true once.  It resets the
      // trigger when it reads true to avoid multiple reads.
    }
    // @sync: end boilerplate
  }

  // Check for event, add to queue if found.
  do {

    try {

    } catch (...) {

      continue;
    }
    usleep(10);

  } while ((rc == false) && (++pollcount < pollmax));

  return 1;
}

//--- Interrupt configuration ---------------------------------------*/        
INT interrupt_configure(INT cmd, INT source, PTYPE adr)
{
  switch (cmd) {
  case CMD_INTERRUPT_ENABLE:
    break;
  case CMD_INTERRUPT_DISABLE:
    break;
  case CMD_INTERRUPT_ATTACH:
    break;
  case CMD_INTERRUPT_DETACH:
    break;
  }
  return SUCCESS;
}

//--- Event readout -------------------------------------------------*/         
INT read_trigger_event(char *pevent, INT off)
{
  int count = 0;
  char bk_name[10];
  double *pdata;

  double magnetic_field;
  string field_unit;
  double time_stamp;

  // @user: readout routine here.                                              
  // And MIDAS output.                                                          
  bk_init32(pevent);

  // Get the time as an example                                                
  sprintf(bk_name, "MTR2");

  bk_create(pevent, bk_name, TID_DOUBLE, &pdata);

  *(pdata++) = 5.;
  
  bk_close(pevent, pdata);

  // @sync: begin boilerplate                                                  
  // Let the trigger listener know we are ready for the next event.  
  if (listener != nullptr) {
    listener->SetReady();
  }
  // @sync: end boilerplate                                                     

  return bk_size(pevent);
};
