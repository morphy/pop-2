#include <iostream>
#include <string>
#include <fstream>
#include <vector>
//#include <windows.h>

using namespace std;

void welcome()
{
  cout << "MW Virtual Machine v1.0" << endl << endl;
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
  cout << " cl - clear screen" << endl;
  cout << " e xxx.bin - execute xxx.bin file" << endl;
}

unsigned short reverse(unsigned short x)
{
  unsigned short tmp;
  tmp = x << 8;
  x >>= 8;
  x += tmp;
  return x;
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

    int memory[64];
    int command;
    char flag;

    /* Binary file data */

    bool is_compiled;
    string binary;
    int size;

  public:
    vm()
    {
      for(int i=0; i<64; i++)
      {
        memory[i] = 0;
      }

      command = 0;
      flag = 0;
      is_compiled = false;
    }

    bool compile(string filename);
    bool execute(string filename);
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
        int r1, r2, number, bytes, s;
        unsigned short buffer;

        bytes = 0;

        while(file_vm >> command)
        {
          /* We have only 2 unusual commands - jum and con */

          if ((command != "jum") && (command !="con"))
          {
            /* For the wr and re command we need only one address */
            /* For the end command we don't need any addresses */

            if((command == "wr") || (command == "re"))
            {
              file_vm >> r1;
              r2 = 0;
            }
            else if(command == "end")
            {
              r1 = 0;
              r2 = 0;
            }
            else
            {
              file_vm >> r1 >> r2;
            }

            /* Write numbers to buffer */

            buffer = 0;
            buffer += r2;
            buffer <<= 6;
            buffer += r1;
            buffer <<= 4;

            /* Command number */

            if(command == "add") buffer += 0;
            if(command == "sub") buffer += 1;
            if(command == "mul") buffer += 2;
            if(command == "div") buffer += 3;
            if(command == "com") buffer += 4;
            if(command == "cp") buffer += 5;
            if(command == "re") buffer += 8;
            if(command == "wr") buffer += 9;
            if(command == "end") buffer += 10;

            /* Write to file */

            file_bin.write((char*)(&buffer), 2);
            bytes += 2;
          }
          else if(command == "jum")
          {
            file_vm >> number >> r1;
            r2 = 0;

            buffer = 0;
            buffer += r2;
            buffer <<= 6;
            buffer += r1;
            buffer <<= 4;

            /* Command number */

            buffer += 6;

            /* Write to file */

            file_bin.write((char*)(&buffer), 2);
            file_bin.write((char*)(&number), 4);
            bytes += 6;
          }
          else if(command == "con")
          {
            file_vm >> r1 >> number;
            r2 = 0;

            buffer = 0;
            buffer += r2;
            buffer <<= 6;
            buffer += r1;
            buffer <<= 4;

            /* Command number */

            buffer += 7;

            /* Write to file */

            file_bin.write((char*)(&buffer), 2);
            file_bin.write((char*)(&number), 4);
            bytes += 6;
          }
        }

        cout << "Compiled" << endl << bytes << " bytes written to " << filename_bin << endl;

        /* Check size */

        file_bin.seekg(0, ios::end);
        s = file_bin.tellg();

        /* That warning should never be emitted, but it's better to know if it would */

        if(s != bytes)
        {
          cout << "Warning: File compled, but sizes are not " << endl;
        }

        file_bin.seekg(0);
        file_bin.close();
        vm::binary = filename_bin;
        vm::is_compiled = true;
        vm::size = s;
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

bool vm::execute(string filename)
{
  /* Check if .bin file */

  int l = filename.length();
  string ext = "0000";

  ext[0] = filename[l-4];
  ext[1] = filename[l-3];
  ext[2] = filename[l-2];
  ext[3] = filename[l-1];

  if(ext == ".bin")
  {
    /* Try to open file */

    fstream file;

    file.open(filename.c_str(), ios::in | ios::binary);

    if(file.good())
    {
      /* Read data */

      vector <int> commands;
      int buffer, size;

      size = vm::size;

      for(int i=0; i<size; i+=2)
      {
        file.read((char*) &buffer, 2);
        file.seekg(i);
        commands.push_back(buffer);
      }








      file.close();
      return true;
    }
    else
    {
      cout << "Can not access binary " << filename << endl;
      return false;
    }
  }
  else
  {
    cout << "Please provide a valid, .bin format file" << endl;
    return false;
  }
}

int main()
{
  /* Declare variables */

  string name, x, cmd;

  /* Display welcome message */

  clear();
  welcome();

  /* Start vm */

  vm machine;

  /* Get user input */

  while(true)
  {
    cout << endl;
    cin >> cmd;

    if(cmd == "?")
    {
      clear();
      help();
    }
    else if(cmd == "q")
    {
      return 0;
    }
    else if(cmd == "cl")
    {
      clear();
      welcome();
    }
    else if(cmd == "e")
    {
      cin >> name;

      machine.execute(name);
    }
    else if(cmd == "c")
    {
      cin >> name;

      machine.compile(name);
    }
    else
    {
      cout << "Unknown command, type ? to display help";
    }
  }

  return 0;
}
