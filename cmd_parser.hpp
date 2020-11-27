#ifndef CMD_PARSER_HPP
#define CMD_PARSER_HPP
#include <cstring>
#include <cstdio>
#include <string>
using std::string;
namespace mine{
    
    //shoud used only for arguments like /name arg or -name arg or arg.
    class cmd_parser{
        const static int MAX_ARG=1<<6;
        char named_args[MAX_ARG][1<<8];
        char unnamed_args[MAX_ARG][1<<8];
    public:
        char names[MAX_ARG][1<<4];
        int nnamed,nunnamed;
        bool ok;
        char log[1<<6];
        cmd_parser(int argc,char **argv):nnamed(0),nunnamed(0)
        {
            //ensure the name is not parse as an unnamed argument.
            ++argv,--argc;

            for(int i=0;i<argc;++i){
                char *curarg=argv[i];
                if(curarg[0]=='/'||curarg[0]=='-'){
                    //solve named arguments.
                    strcpy(names[nnamed],curarg+1);
                    ++i;
                    if(i>=argc){
                        ok=false;
                        sprintf(log,"Argument of %s Not Found\n",static_cast<char *>(names[nnamed]));
                        return;
                    }
                    strcpy(named_args[nnamed++],argv[i]);

                }
                else{
                    //solve unnamed arguments.
                    strcpy(unnamed_args[nunnamed++],curarg);
                }
            }
            ok=true;
        }

        char *operator[](string key)
        {
            //no map use here, O(n)
            for(int i=0;i<nnamed;++i){
                if(!strcmp(key.c_str(),names[i])){
                    return named_args[i];
                }
            }
            return NULL;
        }

        char *operator[](int id)
        {
            if(id>=0&&id<nunnamed)
                return unnamed_args[id];
            else 
                return NULL;
        }
    };
}



#endif