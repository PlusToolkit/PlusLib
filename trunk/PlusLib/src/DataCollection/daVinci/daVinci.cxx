#include "stdafx.h"
#include "daVinci.h"

daVinci::daVinci()
{
  g_print_stream = ISI_FALSE;
  quit = ISI_FALSE;
  manipIndex = ISI_PSM2;
  connected = false;
  status = ISI_SUCCESS;
}

daVinci::~daVinci()
{
  Stop();
}

void daVinci::Stop()
{
  if(connected)
  {
    isi_stop_stream();
    isi_disconnect();
    connected = false;
  }
}

ISI_STATUS daVinci::Connect()
{
  status = connect_with_args();
  if (status != ISI_SUCCESS)
  {
    printf("Failed to Connect to the robot\n");
    isi_stop_stream();
    isi_disconnect();
  }
  connected = true;
  return status;
}

ISI_STATUS daVinci::connect_with_args()
{
  ISI_UINT password = strtol(mPassword, 0, 16);
  return isi_connect_ex(mIpAddr, mPort, password);
}

ISI_STATUS daVinci::Subscribe(ISI_EVENT_CALLBACK eCB, ISI_STREAM_CALLBACK sCB)
{
  // Subscribe to all fields
  status = status && isi_subscribe_all_stream_fields();
  if(sCB != NULL)
  {
    isi_set_stream_callback(sCB, &manipIndex);
  }
  Sleep(500);

  // Subscribe to all events
  status = status && isi_subscribe_all_events();
  if(eCB != NULL)
  {
    isi_set_event_callback(eCB, NULL);  
  }
  Sleep(500);

  // Start streaming information at 60 Hz
  status = status && isi_start_stream(ISI_API_RATE);

  if (status != ISI_SUCCESS)
  {
    printf("Failed to start stream\n");
    printf("Disconnecting from da Vinci\n");
    Stop();
  }
  return status;
}

void daVinci::get_manip_id(ISI_MANIP_INDEX *manipIndex)
{   
  int i;
  int tmp = -1;

  printf("\nSet manip\n");
  printf("\nList of manip indices and names\n");
  for (i = 0; i != ISI_NUM_MANIPS; i++)
  {
    ISI_MANIP_INDEX manipIndex = (ISI_MANIP_INDEX) i;
    printf("%d: %s\n", i, isi_get_manip_name(manipIndex));
  }

  printf("Enter manipulator id [0,%d]: ", ISI_NUM_MANIPS);

  scanf("%d", &tmp);

  if (IS_VALID_MANIP_INDEX(tmp))
  {
    *manipIndex = (ISI_MANIP_INDEX) tmp;
    printf("Manipulator has been set\n\n");
  }
  else
  {
    printf("Invalid manipulator id\n");
  }
}

std::vector<std::string> daVinci::get_manip_names_list()
{
  std::vector<std::string> names;

  for(int i = 0; i != ISI_NUM_MANIPS; i++)
  {
    ISI_MANIP_INDEX index = (ISI_MANIP_INDEX) i;
    std::string name = isi_get_manip_name(index);

    if(name == NULL) continue;

    names.push_back(name);
  }

  return names;
}

void daVinci::get_position(ISI_TRANSFORM* T)
{
  ISI_STREAM_FIELD stream_data; // Datastreams

  isi_get_stream_field(manipIndex, ISI_TIP_TRANSFORM, &stream_data);

  transform_copy((ISI_TRANSFORM *) stream_data.data, T);
}

void daVinci::transform_copy(ISI_TRANSFORM* in, ISI_TRANSFORM* out)
{
  if( in == NULL || out == NULL) return;

  out->pos.x = in->pos.x;
  out->pos.y = in->pos.y;
  out->pos.z = in->pos.z;

  out->rot.row0 = in->rot.row0;
  out->rot.row1 = in->rot.row1;
  out->rot.row2 = in->rot.row2;

}

void daVinci::print_transform(const ISI_TRANSFORM *T)
{
  printf("Pos   : %f %f %f\n", 
    T->pos.x, T->pos.y, T->pos.z);

  printf("X axis: %f %f %f\n", 
    T->rot.row0.x, T->rot.row1.x, T->rot.row2.x);

  printf("Y axis: %f %f %f\n", 
    T->rot.row0.y, T->rot.row1.y, T->rot.row2.y);

  printf("Z axis: %f %f %f\n", 
    T->rot.row0.z, T->rot.row1.z, T->rot.row2.z);
}

void daVinci::print_help()
{
  static const char help[] = {"\
                API Stream Menu \n\
                ---------------- \n\
                Q: Quit          \n\
                M: Set Manip ID  \n\
                V: Print Version \n\
                P: Print Sample  \n\
                S: Print Stream  \n\
                R: Record file   \n\
                "};

  printf("%s\n", help);    
}

std::string daVinci::get_library_version()
{
  ISI_STATUS status;
  ISI_SYSTEM_CONFIG config;    

  status = isi_get_system_config(&config);

  if(status != ISI_SUCCESS)
  {
    return "";
  }

  return config.library_version;
}

bool daVinci::IsConnected()
{
  return connected;
}


///////////////////////////////////////////////////////////////////////////////
void daVinci::print_stream_state(ISI_MANIP_INDEX manipIndex)
{
  ISI_STREAM_FIELD stream_data;
  int i;

  printf("---\n");

  printf("manipIndex: %s\n", isi_get_manip_name(manipIndex));

  printf("\nISI_TIP_TRANSFORM\n");
  isi_get_stream_field(manipIndex, ISI_TIP_TRANSFORM, &stream_data);    
  print_transform((ISI_TRANSFORM *) stream_data.data);
  printf("\n");

  printf("ISI_JOINT_VALUES\n");
  isi_get_stream_field(manipIndex, ISI_JOINT_VALUES, &stream_data);    
  for (i = 0; i != stream_data.count; i++)
  {
    printf("%.3f ", stream_data.data[i]);
  }
  printf("\n");

  printf("\ntimestamp: %.3f (sec)\n", stream_data.timestamp);

  printf("\n\n");    
}

//////////////////////////////////////////////////////////////////////
void daVinci::save_stream_state(ISI_MANIP_INDEX manipIndex)
{

  FILE * pFile;     // The file we will write to
  ISI_STREAM_FIELD stream_data; // Datastreams
  ISI_TRANSFORM *T;   // Manip and Cam transformations
  // The difference between the tip frame and the physical tip

  // Get manipulator transformation from stream
  isi_get_stream_field(manipIndex, ISI_TIP_TRANSFORM, &stream_data);
  T = (ISI_TRANSFORM *) stream_data.data;


  std::string filename = "";
  std::cout << "fimename?" <<std::endl;
  getline(std::cin, filename);
  // Open file for writing, and write the x,y,z data from the transformed point
  pFile = fopen(filename.c_str(),"w");

  fprintf(pFile,"Pos   : %f %f %f\n", 
    T->pos.x, T->pos.y, T->pos.z);

  fprintf(pFile,"X axis: %f %f %f\n", 
    T->rot.row0.x, T->rot.row1.x, T->rot.row2.x);

  fprintf(pFile,"Y axis: %f %f %f\n", 
    T->rot.row0.y, T->rot.row1.y, T->rot.row2.y);

  fprintf(pFile,"Z axis: %f %f %f\n", 
    T->rot.row0.z, T->rot.row1.z, T->rot.row2.z);


  fclose(pFile);

}

///////////////////////////////////////////////////////////////////////////////
void daVinci::print_version()
{
  ISI_STATUS status;
  ISI_SYSTEM_CONFIG config;    

  status = isi_get_system_config(&config);

  printf("\n");

  if (status == ISI_SUCCESS)
  {
    printf("System Name: %s\n", config.system_name);
    printf("System Version: %s\n", config.system_version);
    printf("Library Version: %s\n", config.library_version);
  }
  else
  {
    printf("Failed to get system config\n");
  }

  printf("\n");
}
