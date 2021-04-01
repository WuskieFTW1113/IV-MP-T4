namespace modelsHandler
{
	void request(unsigned long model, long cTime);
	bool wasRequested(unsigned long model);
	void unload(unsigned long model);

	void clearUnsued(long cTime);
}