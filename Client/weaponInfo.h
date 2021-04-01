namespace weaponInfo
{
	struct wInfo
	{
		int shootInterval;
		float range;
		int clipSize;

		wInfo(int shootInterval, float range, int clipSize);
	};

	void initGuns();

	bool isWeapon(int id);
	wInfo* getWeapon(int id);
}