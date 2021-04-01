void initAnimLookUpSequences();
void changeDefaultAnimGroup(char* c);

unsigned int getAnimIdByName(char* group, char* name);

void setServerAnim(size_t animId);
bool doesServerAnimRepeat();
void setServerAnimRepeat(bool b);
unsigned int getServerAnim();

unsigned int getSitting();
void setSittingAnim(std::string group, std::string name, unsigned int so);
void forceSitNow();