#include<iostream>
#include<cstdlib>
#include<fstream>
#include<vector>
#include<regex>
#include<sstream>
#include<map>

#define VERSION "0.01"

using namespace std;

enum instruction_type{
    NOP,ORIG,FILL,BLKW,STRINGZ,ADD1,ADD2,AND1,AND2,
    BR,JMP,JSR,JSRR,LD,LDI,LDR,LEA,NOT,RTI,
    ST,STI,STR,TRAP,END
};

class address;
class instruction;

string to_str(int n);
string to_str(address addr);
string reg_str(int n);
string to_hex(int n);
int to_int(string s);
bool is_num(string s);
void assert_range(int x,int nbit,bool issigned,int line_no);

map<string,int> table;
vector<instruction> insts;

class address{
public:
    address():format(-1){}
    address(string addr){
        if(is_num(addr)){
            format=0;
            num=to_int(addr);
        }else{
            format=1;
            label=addr;
        }
    }
    void replace_label(int PC,int line_no);

    int format;//0:number 1:label -1:invalid
    int num;
    string label;
};

void address::replace_label(int PC,int line_no){
    if(format==1){
        if(table.find(label)==table.end()){
            throw "Label "+label+" not found at line "+to_str(line_no);
        }else{
            format=0;
            num=table[label]-PC-1;
        }
    }
}

class instruction{
public:
    instruction(){}
    instruction(string label,instruction_type type,int n):label(label),type(type){
        switch(type){
            case ORIG:ORIGn=n;break;
            case BLKW:BLKWn=n;break;
            case JMP:BaseR=n;break;
            case JSRR:BaseR=n;break;
            case TRAP:trapvect8=n;break;
            default:throw "Internal Error inst1";
        }
    }
    instruction(string label,instruction_type type,address n):label(label),type(type){
        switch(type){
            case FILL:FILLn=n;break;
            case JSR:PCoffset11=n;break;
            default:throw "Internal Error inst2";
        }
    }
    instruction(string label,instruction_type type):label(label),type(type){
        switch(type){
            case END:break;
            case RTI:break;
            case NOP:break;
            default:throw "Internal Error inst3";
        }
    }
    instruction(string label,instruction_type type,string sz):label(label),type(type){
        switch(type){
            case STRINGZ:STRINGZs=sz;break;
            default:throw "Internal Error inst4";
        }
    }
    instruction(string label,instruction_type type,int n1,int n2,int n3):label(label),type(type){
        switch(type){
            case ADD1:DR=n1,SR1=n2,SR2=n3;break;
            case ADD2:DR=n1,SR1=n2,imm5=n3;break;
            case AND1:DR=n1,SR1=n2,SR2=n3;break;
            case AND2:DR=n1,SR1=n2,imm5=n3;break;
            case LDR:DR=n1,BaseR=n2,offset6=n3;break;
            case STR:SR1=n1,BaseR=n2,offset6=n3;break;
            default:throw "Internal Error inst5";
        }
    }
    instruction(string label,instruction_type type,int n1,address n2):label(label),type(type){
        switch(type){
            case BR:nzp=n1,PCoffset9=n2;break;
            case LD:DR=n1,PCoffset9=n2;break;
            case LDI:DR=n1,PCoffset9=n2;break;
            case LEA:DR=n1,PCoffset9=n2;break;
            case ST:SR1=n1,PCoffset9=n2;break;
            case STI:SR1=n1,PCoffset9=n2;break;
            default:throw "Internal Error inst6";
        }
    }
    instruction(string label,instruction_type type,int n1,int n2):label(label),type(type){
        switch(type){
            case NOT:DR=n1,SR1=n2;break;
            default:throw "Internal Error inst7";
        }
    }

    int length();
    vector<unsigned short> to_int16();
    string to_asm();

    string label;
    instruction_type type;
    int DR,SR1,SR2,nzp,BaseR,imm5,offset6,trapvect8;
    int ORIGn,BLKWn;
    address PCoffset9,PCoffset11,FILLn;
    string STRINGZs;
    int PC,line_no;
};

int instruction::length(){
    switch(type){
        case NOP:return 0;
        case BLKW:return BLKWn;
        case STRINGZ:return STRINGZs.size()+1;
        case END:return 0;
        default:return 1;
    }
}

string instruction::to_asm(){
    switch(type){
        case ORIG:return ".ORIG "+to_str(ORIGn);
        case FILL:return label+" .FILL "+to_str(FILLn);
        case BLKW:return label+" .BLKW "+to_str(BLKWn);
        case END:return ".END";
        case STRINGZ:return label+" .STRINGZ \""+STRINGZs+"\"";
        case ADD1:return label+" ADD "+reg_str(DR)+","+reg_str(SR1)+","+reg_str(SR2);
        case ADD2:return label+" ADD "+reg_str(DR)+","+reg_str(SR1)+","+to_str(imm5);
        case AND1:return label+" AND "+reg_str(DR)+","+reg_str(SR1)+","+reg_str(SR2);
        case AND2:return label+" AND "+reg_str(DR)+","+reg_str(SR1)+","+to_str(imm5);
        case BR:return label+" BR"+(nzp&4?"n":"")+(nzp&2?"z":"")+(nzp&1?"p":"")+" "+to_str(PCoffset9);
        case JMP:return label+" JMP "+reg_str(BaseR);
        case JSR:return label+" JSR "+to_str(PCoffset11);
        case JSRR:return label+" JSRR "+reg_str(BaseR);
        case LD:return label+" LD "+reg_str(DR)+","+to_str(PCoffset9);
        case LDI:return label+" LDI "+reg_str(DR)+","+to_str(PCoffset9);
        case LDR:return label+" LDR "+reg_str(DR)+","+reg_str(BaseR)+","+to_str(offset6);
        case LEA:return label+" LEA "+reg_str(DR)+","+to_str(PCoffset9);
        case NOT:return label+" NOT "+reg_str(DR)+","+reg_str(SR1);
        case RTI:return label+" RTI";
        case ST:return label+" ST "+reg_str(SR1)+","+to_str(PCoffset9);
        case STI:return label+" STI "+reg_str(SR1)+","+to_str(PCoffset9);
        case STR:return label+" STR "+reg_str(SR1)+","+reg_str(BaseR)+","+to_str(offset6);
        case TRAP:return label+" TRAP "+to_str(trapvect8);
        case NOP:return ";";
        default:throw "Unknown type in to_asm";
    }
}

vector<unsigned short> instruction::to_int16(){
    vector<unsigned short> v;
    if(type==NOP||type==END){
        return v;
    }else if(type==BLKW){
        for(int i=0;i<BLKWn;i++)v.push_back(0);
        return v;
    }else if(type==STRINGZ){
        for(unsigned char c:STRINGZs)v.push_back(c);
        v.push_back(0);
        return v;
    }else{
        int imm5_=imm5&0x1F;
        int offset6_=offset6&0x3F;
        int trapvect8_=trapvect8&0xFF;
        int PCoffset9_=PCoffset9.num&0x1FF;
        int PCoffset11_=PCoffset11.num&0x7FF;

        unsigned short x;
        switch(type){
        case ORIG:
            x=ORIGn;
            break;
        case FILL:
            x=FILLn.num;
            break;
        case ADD1:
            x=0x1000|DR<<9|SR1<<6|SR2;
            break;
        case ADD2:
            assert_range(imm5,5,true,line_no);
            x=0x1000|DR<<9|SR1<<6|1<<5|imm5_;
            break;
        case AND1:
            x=0x5000|DR<<9|SR1<<6|SR2;
            break;
        case AND2:
            assert_range(imm5,5,true,line_no);
            x=0x5000|DR<<9|SR1<<6|1<<5|imm5_;
            break;
        case BR:
            assert_range(PCoffset9.num,9,true,line_no);
            x=0x0000|nzp<<9|PCoffset9_;
            break;
        case JMP:
            x=0xC000|BaseR<<6;
            break;
        case JSR:
            assert_range(PCoffset11.num,11,true,line_no);
            x=0x4000|1<<11|PCoffset11_;
            break;
        case JSRR:
            x=0x4000|BaseR<<6;
            break;
        case LD:
            assert_range(PCoffset9.num,9,true,line_no);
            x=0x2000|DR<<9|PCoffset9_;
            break;
        case LDI:
            assert_range(PCoffset9.num,9,true,line_no);
            x=0xA000|DR<<9|PCoffset9_;
            break;
        case LDR:
            assert_range(offset6,6,true,line_no);
            x=0x6000|DR<<9|BaseR<<6|offset6_;
            break;
        case LEA:
            assert_range(PCoffset9.num,9,true,line_no);
            x=0xE000|DR<<9|PCoffset9_;
            break;
        case NOT:
            x=0x9000|DR<<9|SR1<<6|0x3F;
            break;
        case RTI:
            x=0x8000;
            break;
        case ST:
            assert_range(PCoffset9.num,9,true,line_no);
            x=0x3000|SR1<<9|PCoffset9_;
            break;
        case STI:
            assert_range(PCoffset9.num,9,true,line_no);
            x=0xB000|SR1<<9|PCoffset9_;
            break;
        case STR:
            assert_range(offset6,6,true,line_no);
            x=0x7000|SR1<<9|BaseR<<6|offset6_;
            break;
        case TRAP:
            assert_range(trapvect8,8,false,line_no);
            x=0xF000|trapvect8_;
            break;
        default:throw "Unknown instruction type";
        }
        v.push_back(x);
        return v;
    }
}

void assert_range(int x,int nbit,bool issigned,int line_no){
    if(issigned){
        int max=(1<<(nbit-1))-1;
        int min=-(1<<(nbit-1));
        if(x<min||x>max)throw to_hex(x)+" out of range of signed "+to_str(nbit)+" bits at line "+to_str(line_no);
    }else{
        int max=(1<<nbit)-1;
        int min=0;
        if(x<min||x>max)throw to_hex(x)+" out of range of unsigned "+to_str(nbit)+" bits at line "+to_str(line_no);
    }
}

string unescape(string s){
    string r("");
    for(size_t i=0;i<s.size();i++){
        if(s[i]=='\\'&&i<s.size()-1){
            i++;
            char t;
            switch(s[i]){
                case 'n':t='\n';break;
                case 't':t='\t';break;
                default:t=s[i];
            }
            r+=t;
        }else{
            r+=s[i];
        }
    }
    return r;
}

int to_int(string s){
    if(s[0]=='x'){
        s=s.substr(1);
        int r=0;
        for(size_t i=0;i<s.size();i++){
            r<<=4;
            r|=(s[i]<='9')?(s[i]-'0'):(s[i]<='F')?(s[i]-'A'+10):(s[i]-'a'+10);
        }
        return r;
    }else{
        if(s[0]=='#')
            s=s.substr(1);
        int sign=1;
        if(s[0]=='-'){
            s=s.substr(1);
            sign=-1;
        }
        int r=0;
        for(size_t i=0;i<s.size();i++){
            r*=10;
            r+=s[i]-'0';
        }
        r*=sign;
        return r;
    }
}

string to_str(int n){
    stringstream ss;
    string r;
    ss<<n;
    ss>>r;
    return "#"+r;
}

string to_hex(int n){
    stringstream ss;
    string r;
    ss<<hex<<n;
    ss>>r;
    return "x"+r;
}

string to_str(address addr){
    if(addr.format==0)
        return to_str(addr.num);
    else if(addr.format==1)
        return addr.label;
    else
        throw "invalid address converting to string";
}

string reg_str(int n){
    stringstream ss;
    string r;
    ss<<n;
    ss>>r;
    return "R"+r;
}


bool is_num(string s){
    static regex r_num("((x[0-9A-Fa-f]+)|(#?-?[0-9]+))",regex::icase);
    return regex_match(s,r_num);
}

int parse_nzp(string s){
    int r=0;
    for(char c:s){
        switch(c){
            case 'n':case 'N':r|=4;break;
            case 'z':case 'Z':r|=2;break;
            case 'p':case 'P':r|=1;break;
            default:throw "nzp parse error";
        }
    }
    if(r==0)r=7;
    return r;
}

int parse_trap(string s){
    for(char &c:s){
        if(c>='a'&&c<='z')c-=32;
    }
    if(s=="GETC")return 0x20;
    else if(s=="OUT")return 0x21;
    else if(s=="PUTS")return 0x22;
    else if(s=="IN")return 0x23;
    else if(s=="PUTSP")return 0x24;
    else if(s=="HALT")return 0x25;
    else throw "unknown trap";
}

instruction process_line(string line,int line_no){
    static string trim="\\s*";
    static string space="\\s+";
    static string comment=trim+"(;[\\s\\S]*)?";
    static string number="((x[0-9A-Fa-f]+)|(#?-?[0-9]+))";
    static string str="(\"((?:[^\"\\\\]|\\\\.)*)\")";
    static string comma=trim+","+trim;
    static string reg="R([0-9]+)";
    static string lbl="(([A-Za-z_][A-Za-z0-9_]*)\\s+|)";
    static string label="([A-Za-z_][A-Za-z0-9_]*)";
    static string addr="("+number+"|"+label+")";

    static regex r_empty_line(trim+comment,regex::icase);
    static regex r_label_line(trim+label+comment,regex::icase);
    static regex r_ORIG(trim+"\\.ORIG"+space+number+comment,regex::icase);
    static regex r_FILL(trim+lbl+"\\.FILL"+space+addr+comment,regex::icase);
    static regex r_BLKW(trim+lbl+"\\.BLKW"+space+number+comment,regex::icase);
    static regex r_END(trim+"\\.END"+comment,regex::icase);
    static regex r_STRINGZ(trim+lbl+"\\.STRINGZ"+space+str+comment,regex::icase);
    static regex r_ADD1(trim+lbl+"ADD"+space+reg+comma+reg+comma+reg+comment,regex::icase);
    static regex r_ADD2(trim+lbl+"ADD"+space+reg+comma+reg+comma+number+comment,regex::icase);
    static regex r_AND1(trim+lbl+"AND"+space+reg+comma+reg+comma+reg+comment,regex::icase);
    static regex r_AND2(trim+lbl+"AND"+space+reg+comma+reg+comma+number+comment,regex::icase);
    static regex r_BR(trim+lbl+"BR(n?z?p?)"+space+addr+comment,regex::icase);
    static regex r_JMP(trim+lbl+"JMP"+space+reg+comment,regex::icase);
    static regex r_JSR(trim+lbl+"JSR"+space+addr+comment,regex::icase);
    static regex r_JSRR(trim+lbl+"JSRR"+space+reg+comment,regex::icase);
    static regex r_LD(trim+lbl+"LD"+space+reg+comma+addr+comment,regex::icase);
    static regex r_LDI(trim+lbl+"LDI"+space+reg+comma+addr+comment,regex::icase);
    static regex r_LDR(trim+lbl+"LDR"+space+reg+comma+reg+comma+number+comment,regex::icase);
    static regex r_LEA(trim+lbl+"LEA"+space+reg+comma+addr+comment,regex::icase);
    static regex r_NOT(trim+lbl+"NOT"+space+reg+comma+reg+comment,regex::icase);
    static regex r_RET(trim+lbl+"RET"+comment,regex::icase);
    static regex r_RTI(trim+lbl+"RTI"+comment,regex::icase);
    static regex r_ST(trim+lbl+"ST"+space+reg+comma+addr+comment,regex::icase);
    static regex r_STI(trim+lbl+"STI"+space+reg+comma+addr+comment,regex::icase);
    static regex r_STR(trim+lbl+"STR"+space+reg+comma+reg+comma+number+comment,regex::icase);
    static regex r_TRAP(trim+lbl+"TRAP"+space+number+comment,regex::icase);
    static regex r_TRAPS(trim+lbl+"((GETC)|(OUT)|(PUTS)|(IN)|(PUTSP)|(HALT))"+comment,regex::icase);

    if(line[line.size()-1]=='\r')line=line.substr(0,line.size()-1);

    smatch matches;
    instruction inst;

    if(regex_match(line,matches,r_ORIG)){
        inst=instruction("",ORIG,to_int(matches[1]));
    }else if(regex_match(line,matches,r_FILL)){
        inst=instruction(matches[2],FILL,address(matches[3]));
    }else if(regex_match(line,matches,r_BLKW)){
        inst=instruction(matches[2],BLKW,to_int(matches[3]));
    }else if(regex_match(line,matches,r_END)){
        inst=instruction("",END);
    }else if(regex_match(line,matches,r_STRINGZ)){
        inst=instruction(matches[2],STRINGZ,unescape(matches[4]));
    }else if(regex_match(line,matches,r_ADD1)){
        inst=instruction(matches[2],ADD1,to_int(matches[3]),to_int(matches[4]),to_int(matches[5]));
    }else if(regex_match(line,matches,r_ADD2)){
        inst=instruction(matches[2],ADD2,to_int(matches[3]),to_int(matches[4]),to_int(matches[5]));
    }else if(regex_match(line,matches,r_AND1)){
        inst=instruction(matches[2],AND1,to_int(matches[3]),to_int(matches[4]),to_int(matches[5]));
    }else if(regex_match(line,matches,r_AND2)){
        inst=instruction(matches[2],AND2,to_int(matches[3]),to_int(matches[4]),to_int(matches[5]));
    }else if(regex_match(line,matches,r_BR)){
        inst=instruction(matches[2],BR,parse_nzp(matches[3]),address(matches[4]));
    }else if(regex_match(line,matches,r_JMP)){
        inst=instruction(matches[2],JMP,to_int(matches[3]));
    }else if(regex_match(line,matches,r_JSR)){
        inst=instruction(matches[2],JSR,address(matches[3]));
    }else if(regex_match(line,matches,r_JSRR)){
        inst=instruction(matches[2],JSRR,to_int(matches[3]));
    }else if(regex_match(line,matches,r_LD)){
        inst=instruction(matches[2],LD,to_int(matches[3]),address(matches[4]));
    }else if(regex_match(line,matches,r_LDI)){
        inst=instruction(matches[2],LDI,to_int(matches[3]),address(matches[4]));
    }else if(regex_match(line,matches,r_LDR)){
        inst=instruction(matches[2],LDR,to_int(matches[3]),to_int(matches[4]),to_int(matches[5]));
    }else if(regex_match(line,matches,r_LEA)){
        inst=instruction(matches[2],LEA,to_int(matches[3]),address(matches[4]));
    }else if(regex_match(line,matches,r_NOT)){
        inst=instruction(matches[2],NOT,to_int(matches[3]),to_int(matches[4]));
    }else if(regex_match(line,matches,r_RET)){
        inst=instruction(matches[2],JMP,7);
    }else if(regex_match(line,matches,r_RTI)){
        inst=instruction(matches[2],RTI);
    }else if(regex_match(line,matches,r_ST)){
        inst=instruction(matches[2],ST,to_int(matches[3]),address(matches[4]));
    }else if(regex_match(line,matches,r_STI)){
        inst=instruction(matches[2],STI,to_int(matches[3]),address(matches[4]));
    }else if(regex_match(line,matches,r_STR)){
        inst=instruction(matches[2],STR,to_int(matches[3]),to_int(matches[4]),to_int(matches[5]));
    }else if(regex_match(line,matches,r_TRAP)){
        inst=instruction(matches[2],TRAP,to_int(matches[3]));
    }else if(regex_match(line,matches,r_TRAPS)){
        inst=instruction(matches[2],TRAP,parse_trap(matches[3]));
    }else if(regex_match(line,matches,r_label_line)){
        inst=instruction(matches[1],NOP);
    }else if(regex_match(line,matches,r_empty_line)){
        inst=instruction("",NOP);
        /*for(int i=0;i<matches.size();i++){
            cout<<"Match "<<i<<": $"<<matches[i]<<"$"<<endl;
        }*/
    }else{
        throw "Syntax Error at line "+to_str(line_no);
    }

    return inst;
}

void first_scan(string infile_name){
    ifstream infile(infile_name);
    if(!infile){
        cout<<"Cannot open file: "<<infile_name<<endl;
        exit(1);
    }
    string line;
    int line_no=1;
    int PC=-1;
    bool after_end=false;
    while(getline(infile,line)){
        //cout<<"line#"<<line_no<<": "<<line<<endl;
        instruction inst=process_line(line,line_no);
        //cout<<"Parsed: "<<inst.to_asm()<<endl;

        if((inst.type!=ORIG&&inst.type!=NOP&&PC==-1)||(inst.label!=""&&PC==-1)){
            throw ".ORIG Not found at line "+to_str(line_no);
        }
        if(inst.type==ORIG&&PC==-1){
            PC=inst.ORIGn-1;
        }

        if(inst.label!=""){
            if(table.find(inst.label)!=table.end()){
                throw "Label "+inst.label+" duplicated at line "+to_str(line_no);
            }else{
                table[inst.label]=PC;
            }
        }

        inst.PC=PC;
        inst.line_no=line_no;
        insts.push_back(inst);

        PC+=inst.length();
        line_no++;

        if(inst.type==END){
            after_end=true;
            break;
        }
    }
    if(!after_end)
        throw ".END not found in input file";
    infile.close();
}

void second_scan(){
    for(instruction &inst:insts){
        inst.PCoffset9.replace_label(inst.PC,inst.line_no);
        inst.PCoffset11.replace_label(inst.PC,inst.line_no);
        inst.FILLn.replace_label(-1,inst.line_no);
    }
}

void save_file(string outfile_name,string symfile_name){
    ofstream outfile(outfile_name,ios::binary);
    if(!outfile){
        cout<<"Cannot open file: "<<outfile_name<<endl;
        exit(1);
    }
    ofstream symfile(symfile_name);
    if(!symfile){
        cout<<"Cannot open file: "<<symfile_name<<endl;
        outfile.close();
        exit(1);
    }

    for(instruction inst:insts){
        vector<unsigned short> v=inst.to_int16();
        for(unsigned short x:v){
            x=x>>8|x<<8;
            outfile.write((char*)&x,sizeof(unsigned short));
        }
        if(v.size()>0&&inst.type!=ORIG){
            symfile<<inst.line_no<<" "<<to_hex(inst.PC)<<endl;
        }
        if(inst.label!=""){
            symfile<<inst.label<<" "<<to_hex(inst.PC)<<endl;
        }
    }
    outfile.close();
    symfile.close();
}

void show_help(){
    cout<<
    "LC3 Assembler Ver" VERSION "\n"
    "Usage: lc3asm file.asm [-o file.obj]\n"
    "Author: zzh1996@mail.ustc.edu.cn http://sqrt-1.me/\n"
    "Sep. 2015"
    <<endl;
}

string change_ext(string file_name,string ext){
    size_t slash_pos=file_name.find_last_of("/\\");
    size_t dot_pos=file_name.find_first_of(".",slash_pos+1);
    if(dot_pos==string::npos){
        return file_name+"."+ext;
    }else{
        return file_name.replace(dot_pos+1,file_name.size()-dot_pos-1,ext);
    }
}

int main(int argc, char const *argv[])
{
    try{
        string infile_name,outfile_name,symfile_name;
        if(argc==4&&string(argv[2])=="-o"){
            infile_name=argv[1];
            outfile_name=argv[3];
            symfile_name=change_ext(outfile_name,"sym");
        }else if(argc==2){
            infile_name=argv[1];
            outfile_name=change_ext(infile_name,"obj");
            symfile_name=change_ext(outfile_name,"sym");
        }else{
            show_help();
            exit(0);
        }
        first_scan(infile_name);
        second_scan();
        //for(instruction inst:insts){cout<<inst.line_no<<" "<<inst.PC<<" "<<inst.to_asm()<<endl;}
        save_file(outfile_name,symfile_name);
        cout<<"Succeeded!"<<endl;
    }catch(string s){
        cout<<s<<endl;
        cout<<"Assembling terminated!"<<endl;
        exit(1);
    }catch(const char s[]){
        cout<<s<<endl;
        cout<<"Assembling terminated!"<<endl;
        exit(1);
    }catch(...){
        cout<<"Unknown internal error!"<<endl;
        exit(1);
    }
    return 0;
}