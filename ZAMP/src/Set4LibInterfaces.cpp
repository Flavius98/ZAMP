#include "Set4LibInterfaces.hh"

Set4LibInterfaces::Set4LibInterfaces(std::map<std::string, MobileObj*> &_Object_List)
{
    this->scena = new Scena(_Object_List);
    LoadLibrary("libs/libInterp4Move.so");
    LoadLibrary("libs/libInterp4Set.so");
    LoadLibrary("libs/libInterp4Rotate.so");
    LoadLibrary("libs/libInterp4Pause.so");
    //this->scena->AddMobileObj("Alpha");
}

Set4LibInterfaces::~Set4LibInterfaces() {_Lib_List.clear();if(nullptr != scena) delete scena;}

void Set4LibInterfaces::LoadLibrary(std::string path)
{
    LibInterface *pLibrary = new LibInterface(path);
    _Lib_List.insert({pLibrary->_Lib_Name, pLibrary});
}

bool Set4LibInterfaces::ExecPreprocessor(const char *NazwaPliku, std::istringstream &IStrm4Cmds)
{
  std::string Cmd4Preproc = "cpp -P ";
  char Line[500];
  std::ostringstream OTmpStrm;
  std::string test;

  Cmd4Preproc += NazwaPliku;
  FILE* pProc = popen(Cmd4Preproc.c_str(),"r");

  if (!pProc) return false;

  while (fgets(Line, 500, pProc))
  {
    OTmpStrm << Line;
  }

  IStrm4Cmds.str(OTmpStrm.str());

while(IStrm4Cmds >> test)
std::cout << test << endl;

  return pclose(pProc) == 0;
}

bool Set4LibInterfaces::ReadCommands(std::istringstream &iStrm, int socket)
{
  std::string _Com_Name, _Obj_Name;
  bool flag;

  std::vector<MobileObj*> _objectsList = this->getScena()->getObjects();
  
  Send(socket,"Clear\n");
  for(MobileObj* _objectPtr : _objectsList)
  {
  std::string message =  _objectPtr->returnParameters();
 
    

    /*std::string message =  "Clear\n"
  "AddObj Name=Podstawa RGB=(20,200,200) Scale=(4.000000,2.000000,1.000000) Shift=(0.500000,0.000000,0.000000) RotXYZ_deg=(0.000000,-45.000000,20.000000), Trans_m=(-1.000000,3.000000,0.000000)\n"
  "AddObj Name=Podstawa.Ramie1 RGB=(200,0,0) Scale=(3.000000,3.000000,1.000000) Shift=(0.500000,0.000000,0.000000) RotXYZ_deg=(0.000000,-45.000000,0.000000), Trans_m=(4.000000,0.000000,0.000000)\n"
  "AddObj Name=Podstawa.Ramie1.Ramie2 RGB=(100,200,0) Scale=(2.000000,2.000000,1.000000) Shift=(0.500000,0.000000,0.000000) RotXYZ_deg=(0.000000,-45.000000,0.000000), Trans_m=(3.000000,0.000000,0.000000)\n";
 */

/*std::string message =  "Clear\n"
    "AddObj Name=Podstawa RGB=(20,200,200) Scale=(4,2,1) Shift=(0.5,0,0) RotXYZ_deg=(0,-45,20) Trans_m=(-1,3,0)\n"
  "AddObj Name=Podstawa.Ramie1 RGB=(200,0,0) Scale=(3,3,1) Shift=(0.5,0,0) RotXYZ_deg=(0,-45,0) Trans_m=(4,0,0)\n"
  "AddObj Name=Podstawa.Ramie1.Ramie2 RGB=(100,200,0) Scale=(2,2,1) Shift=(0.5,0,0) RotXYZ_deg=(0,-45,0) Trans_m=(3,0,0)\n";   
  */
  Send(socket,message.c_str());
    std::cout<<message<<std::endl;   
    std::cout<< "Panie Janie Pora wstac" << std::endl;

    /*  const char *test = message.c_str();
    Send(socket,test);
    std::cout<<message<<std::endl;*/

  }

  while (iStrm >> _Com_Name) // sprawdza, czy w strumieniu jest jeszcze jakaś komenda
  {
    if(_Com_Name!= "Pause") iStrm >> _Obj_Name;
    
    flag = true;
    std::map<std::string, LibInterface*>::iterator Iter = this->_Lib_List.find(_Com_Name);
    if (Iter == this->_Lib_List.end())
    {
      std::cerr << "!!! Brak komendy " << _Com_Name << std::endl;
      flag = false;
    }
      
      
    

    // Tworzy komendę na podstawie funkcji wtyczki
    if(flag)
    {
    Interp4Command *pCommand = Iter->second->pCreateCmd();
    if (!pCommand->ReadParams(iStrm))
    {
      std::cerr << "!!! Nieporawne parametry dla komendy '" << _Com_Name << std::endl;
      delete pCommand;
      return false;
    }
    MobileObj* _Obj = this->scena->FindMobileObj(_Obj_Name);
    //std::cout << _Obj_Name<<std::endl;
    if(_Obj == nullptr)
    {
      std::cerr << "!!! Obiekt " << _Obj_Name <<" nie istnieje"<< std::endl;
      delete pCommand;
      return false;
    }
    else
   
    pCommand->PrintCmd();
    //if(pCommand->ExecCmd(_Obj, socket)){std::cout<<"Udało się załodować obiekt z sceny"<<std::endl;}
    delete pCommand;
    }
    
  }
  return true;
}