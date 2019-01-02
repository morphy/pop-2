#include <iostream>
#include <string>
#include <fstream>
//#include <windows.h>

using namespace std;

void welcome()
{
  cout << "MW Virtual Machine v1.0" << endl << endl;
  cout << "Type '?' to display help" << endl;
}

void clear()
{
  //system("cls");
  cout << "\033[2J\033[1;1H";
}

void help()
{
  cout << "MW Virtual Machine v1.0" << endl << endl;
  cout << " ? - display help" << endl;
  cout << " c xxx.vm - compile xxx.vm file" << endl;
  cout << " ? - display help" << endl;
  cout << " ? - display help" << endl;
}

string dec2bin(int x)
{
  string res;
  char c;

  while(x > 0)
  {
    c = (x % 2) + 48;
    res = c + res;
    x /= 2;
  }

  return res;
}

string dec2bin(int x, int l)
{
  string res;
  char c;

  while(x > 0)
  {
    c = (x % 2) + 48;
    res = c + res;
    x /= 2;
  }

  int add = l - res.length();

  for(int i=0; i<add; i++)
  {
    res = '0' + res;
  }

  return res;
}

class vm
{
  private:
    /* Registers */

    int memory[63];
    int command;
    char flag;

  public:
    vm()
    {
      for(int i=0; i<64; i++)
      {
        memory[i] = 0;
      }

      command = 0;
      flag = 0;
    }

    bool compile(string filename);
};

bool vm::compile(string filename)
{
  int l = filename.length();
  string ext = "000";
  string filename_bin = filename;

  /* Genrate binary file name */

  filename_bin[l-2] = 'b';
  filename_bin[l-1] = 'i';
  filename_bin += 'n';

  /* Check if .vm file */

  ext[0] = filename[l-3];
  ext[1] = filename[l-2];
  ext[2] = filename[l-1];

  if(ext == ".vm")
  {
    /* Try to open files */

    fstream file_vm, file_bin;

    file_vm.open(filename.c_str(), ios::in);

    if(file_vm.good())
    {
      file_bin.open(filename_bin.c_str(), ios::out |ios::trunc | ios::binary);

      if(file_bin.good())
      {
        /* Compile */

        string command;
        int r1, r2;
        unsigned int buffer;

        while(file_vm >> command)
        {
          if(command == "add")
          {
            file_vm >> r1 >> r2;
            cout << "add " << dec2bin(r1) << " " << r2 << endl;

            buffer = 0;
            buffer += r2;
            buffer <<= 6;
            buffer += r1;
            buffer <<= 4;
            buffer += 0;
            
            file_bin.write((char*)(&buffer), 2);
          }
          else if(command == "sub")
          {
            file_vm >> r1 >> r2;
            cout << "sub " << r1 << " " << r2 << endl;
          }
          else if(command == "mul")
          {
            file_vm >> r1 >> r2;
            cout << "mul " << r1 << " " << r2 << endl;
          }
          else if(command == "div")
          {
            file_vm >> r1 >> r2;
            cout << "div " << r1 << " " << r2 << endl;
          }
          else if(command == "com")
          {
            file_vm >> r1 >> r2;
            cout << "com " << r1 << " " << r2 << endl;
          }
          else if(command == "cp")
          {
            file_vm >> r1 >> r2;
            cout << "cp " << r1 << " " << r2 << endl;
          }
          else if(command == "jum")
          {
            file_vm >> r1;
            cout << "jum " << r1 << endl;
          }
          else if(command == "con")
          {
            file_vm >> r1;
            cout << "con " << r1 << endl;
          }
          else if(command == "re")
          {
            file_vm >> r1 >> r2;
            cout << "re " << r1 << " " << r2 << endl;
          }
          else if(command == "wr")
          {
            file_vm >> r1 >> r2;
            cout << "wr " << r1 << " " << r2 << endl;
          }
          else if(command == "end")
          {
            cout << "end " << r1 << " " << r2 << endl;
          }
          else
          {
            cout << "unknown command" << endl;
          }
        }

        file_bin.close();
        return true;
      }
      else
      {
        cout << "Can not access " << filename_bin << endl;
        return false;
      }

      file_vm.close();
    }
    else
    {
      cout << "Can not access " << filename << endl;
      return false;
    }
  }
  else
  {
    cout << "Please provide a valid, .vm format file" << endl;
    return false;
  }
}















int main()
{
  /* Declare variables */

  char cmd;
  string name,x;

  /* Display welcome message */

  clear();
  welcome();

  /* Start vm */

  vm machine;

  while(true)
  {
    cout << endl;
    cin >> cmd;

    if(cmd == '?')
    {
      clear();
      help();
    }

    if(cmd == 'q')
    {
      return 0;
    }

    if(cmd == 'c')
    {
      cin >> name;

      if(machine.compile(name))
      {
        cout << "Compiled";
      }
      else
      {
        cout << "Compilation failed";
      }
    }
  }

  return 0;
}
