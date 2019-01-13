#include <iostream>
#include <string>
#include <fstream>
#include <vector>
//#include <windows.h> /* For Windows */

using namespace std;

void welcome()
{
  cout << "MW Virtual Machine v1.0" << endl << endl;
}

void clear()
{
  //system("cls"); /* For Windows */
  cout << "\033[2J\033[1;1H"; /* For Linux */
}

void help()
{
  cout << "MW Virtual Machine v1.0" << endl << endl;
  cout << " ?         - display help" << endl;
  cout << " cl        - clear screen" << endl;
  cout << " q         - quit application" << endl;
  cout << " c xxx.vm  - compile xxx.vm file" << endl;
  cout << " e xxx.bin - execute xxx.bin file" << endl;
  cout << " d xxx.bin - debug xxx.bin file" << endl;
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
  struct c
  {
    int r1, r2, op;
  };

  private:
    /* Registers */

    int memory[64];
    int line;
    char flag;

  public:
    vm()
    {
      reset();
    }

    void reset()
    {
      for(int i=0; i<64; i++)
      {
        memory[i] = 0;
      }

      line = 0;
      flag = 'Z';
    }

    bool compile(string filename);
    bool execute(string filename, bool debug);
    bool validate(string filename);
    char getflag(int x);
};

char vm::getflag(int x)
{
  if(x == 0)
  {
    return 'Z';
  }
  else if(x > 0)
  {
    return 'D';
  }
  else if(x < 0)
  {
    return 'U';
  }
}

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
    /* Validate file */

    if(!validate(filename))
    {
      cout << "Compilation failed" << endl;
      return false;
    }

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

bool vm::execute(string filename, bool debug = false)
{
  /* Reset the VM */

  this->reset();

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
      /* Clear screen */

      clear();
      welcome();

      /* Read data */

      vector <c> commands;
      c tmp;
      int buffer, size;
      int r1, r2, op;

      file.seekg(0, ios::end);
      size = file.tellg();
      file.seekg(0);

      /* Rewrite table */

      for(int i=0; i<size; i+=2)
      {
        file.seekg(i);
        file.read((char*) &buffer, 2);

        tmp.op = buffer & 15; // 15 = 0b1111
        tmp.r1 = (buffer & 1008) >> 4; // 1008 = 0b1111110000
        tmp.r2 = (buffer & 64512) >> 10; // 64512 = 0b1111110000000000

        if((tmp.op == 6) || (tmp.op == 7))
        {
          i+=2;
          file.seekg(i);
          file.read((char*) &buffer, 4);
          i+=2;
          tmp.r2 = buffer;
        }

        commands.push_back(tmp);
      }

      /* Execute */

      for(int i=0; i<commands.size(); i++)
      {
        r1 = commands[i].r1;
        r2 = commands[i].r2;
        op = commands[i].op;

        if(op == 0)
        {
          memory[r1] += memory[r2];
          flag = getflag(memory[r1]);
          line++;

          if(debug) cout << "Operation: " << op << " R1: " << r1 << " R2: " << r2 << " Line: " << line << endl;
        }
        else if(op == 1)
        {
          memory[r1] -= memory[r2];
          flag = getflag(memory[r1]);
          line++;

          if(debug) cout << "Operation: " << op << " R1: " << r1 << " R2: " << r2 << " Line: " << line << endl;
        }
        else if(op == 2)
        {
          memory[r1] *= memory[r2];
          flag = getflag(memory[r1]);
          line++;

          if(debug) cout << "Operation: " << op << " R1: " << r1 << " R2: " << r2 << " Line: " << line << endl;
        }
        else if(op == 3)
        {
          if(memory[r2] != 0)
          {
            memory[r1] /= memory[r2];
            memory[r2] = memory[r1] % memory[r2];
            flag = getflag(memory[commands[i].r1]);
            line++;

            if(debug) cout << "Operation: " << op << " R1: " << r1 << " R2: " << r2 << " Line: " << line << endl;
          }
          else
          {
            if(debug) cout << "Operation: " << op << " R1: " << r1 << " R2: " << r2 << " Line: " << line << endl;
            cout << "Division by zero detected" << endl << "Execution terminated";
            return false;
          }
        }
        else if(op == 4)
        {
          flag = getflag(memory[r1] - memory[r2]);
          line++;
          if(debug) cout << "Operation: " << op << " R1: " << r1 << " R2: " << r2 << " Line: " << line << endl;
        }
        else if(op == 5)
        {
          memory[r1] = memory[r2];
          line++;
          if(debug) cout << "Operation: " << op << " R1: " << r1 << " R2: " << r2 << " Line: " << line << endl;
        }
        else if(op == 6)
        {
          if(r1 == 0)
          {
            line += r2;
            i = line - 1;
          }
          else if((r1 == 1) && (flag == 'Z'))
          {
            line += r2;
            i = line - 1;
          }
          else if((r1 == 2) && (flag != 'Z'))
          {
            line += r2;
            i = line - 1;
          }
          else if((r1 == 3) && (flag == 'D'))
          {
            line += r2;
            i = line - 1;
          }
          else if((r1 == 4) && (flag == 'U'))
          {
            line += r2;
            i = line - 1;
          }
          else if((r1 == 5) && ((flag == 'D') || (flag == 'Z')))
          {
            line += r2;
            i = line - 1;
          }
          else if((r1 == 6) && ((flag == 'U') || (flag == 'Z')))
          {
            line += r2;
            i = line - 1;
          }
          else
          {
            line++;
          }

          if(debug) cout << "Operation: " << op << " R1: " << r1 << " R2: " << r2 << " Line: " << line << endl;
        }
        else if(op == 7)
        {
          memory[r1] = r2;
          line++;

          if(debug) cout << "Operation: " << op << " R1: " << r1 << " R2: " << r2 << " Line: " << line << endl;
        }
        else if(op == 8)
        {
          cin >> memory[r1];
          line++;

          if(debug) cout << "Operation: " << op << " R1: " << r1 << " R2: " << r2 << " Line: " << line << endl;
        }
        else if(op == 9)
        {
          cout << memory[r1] << endl;
          line++;

          if(debug) cout << "Operation: " << op << " R1: " << r1 << " R2: " << r2 << " Line: " << line << endl;
        }
        else if(op == 10)
        {
          line++;

          if(debug) cout << "Operation: " << op << " R1: " << r1 << " R2: " << r2 << " Line: " << line << endl;
          break;
        }
      }

      cout << "Execution finished" << endl;

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

bool vm::validate(string filename)
{
  fstream file;
  string command;
  int a, b;
  int line = 0;

  file.open(filename.c_str(), ios::in);

  while(file >> command)
  {
    line ++;

    if((command == "add")
     || (command == "sub")
     || (command == "mul")
     || (command == "div")
     || (command == "com")
     || (command == "cp"))
    {
      if(file >> a >> b)
      {
        if(!((a >= 0) && (a <= 63) && (b >= 0) && (b <= 63)))
        {
          cout << "Error on line " << line << endl;
          cout << "Register address must be between 0 and 63" << endl;
          return false;
        }
      }
      else
      {
        cout << "Error on line " << line << endl;
        cout << "Not a number" << endl;
        return false;
      }
    }
    else if(command == "jum")
    {
      if(file >> a >> b)
      {
        if(!((b >= 0) && (b <= 6)))
        {
          cout << "Error on line " << line << endl;
          cout << "Jump mode must be between 0 and 6" << endl;
          return false;
        }

      }
      else
      {
        cout << "Error on line " << line << endl;
        cout << "Not a number or int" << endl;
        return false;
      }
    }
    else if(command == "con")
    {
      if(file >> a >> b)
      {
        if(!((a <= 63) && (a >= 0)))
        {
          cout << "Error on line " << line << endl;
          cout << "Register address must be between 0 and 63" << endl;
          return false;
        }
      }
      else
      {
        cout << "Error on line " << line << endl;
        cout << "Not a number or int" << endl;
        return false;
      }
    }
    else if((command == "re") || (command == "wr"))
    {
      if(file >> a)
      {
        if(!((a >= 0) && (a <= 63)))
        {
          cout << "Error on line " << line << endl;
          cout << "Register address must be between 0 and 63" << endl;
          return false;
        }
      }
      else
      {
        cout << "Error on line " << line << endl;
        cout << "Not a number" << endl;
        return false;
      }

    }
    else if(command != "end")
    {
      cout << "Error on line " << line << endl;
      cout << "Unknown command" << endl;
      return false;
    }
  }

  return true;
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
    else if(cmd == "d")
    {
      cin >> name;

      machine.execute(name, true);
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
