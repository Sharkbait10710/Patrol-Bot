//Byproduct of discovering JSON 
#ifndef Regex_H
#define Regex_H
#include <Regexp.h> //Utilizes Lua regular expressions
#include <Vector.h>

bool reg_DEBUG = false;

Vector<String> Data;
bool flush = false;
void match_callback  (const char * match,          // matching string (not null-terminated)
                      const unsigned int length,   // length of matching string
                      const MatchState & ms)      // MatchState in use (to get captures)
{
  char cap [10];   // must be large enough to hold captures
  
  Serial.print ("Matched: ");
  Serial.write ((byte *) match, length);
  Serial.println ();
  
  for (byte i = 0; i < ms.level; i++)
    {
    if (reg_DEBUG) {
      Serial.print ("Capture "); 
      Serial.print (i, DEC);
      Serial.print (" = ");
    }
    ms.GetCapture (cap, i);
    Data.push_back(String(cap));
    if (reg_DEBUG) Serial.println (cap); 
    }  // end of for each capture

}  // end of match_callback 

bool Capt_Reg(const char* Lua_Regex, String str) {
  if (!flush) {
    unsigned int Size = sizeof(str);
    char buf[Size];
    str.toCharArray(buf, Size);
    MatchState ms(buf, Size);
    ms.GlobalMatch(Lua_Regex, match_callback);
    if (Data.empty()) return false;
    flush = true;
    return true;
  }
  if (reg_DEBUG) Serial.println("Need to flush data");
  return false;
  //Look in Vector for matched strings
}

String get_Data() {
  if (!Data.empty()) {
    String ret = Data[0];
    Data.remove(0);
    return ret;
  }
  flush = false;
  return "Data has flushed";
}

void flush_Data() {
  while (!Data.empty()) Data.pop_back();
  flush = false;
}
#endif
