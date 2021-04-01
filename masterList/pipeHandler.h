namespace pipeHandler
{
	void init(int id, const char* name);
	void write(int id, const char* s);
	const char* read(int id);
}