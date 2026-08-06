#pragma once



class FPSCombatShotTracker
{
public:
	void Reset()
	{
		OutstandingShots = 0;
		bDoneFiring = false;
	}

	void NotifyShotFired()
	{
		++OutstandingShots;
	}

	void NotifyShotResolved()
	{
		checkf(OutstandingShots > 0, TEXT("NotifyShotResolved called with no outstanding shots"));
		--OutstandingShots;
	}

	void NotifyShotDone()
	{
		bDoneFiring = true;
	}
	

	bool IsActivationComplete() const
	{
		return (bDoneFiring && OutstandingShots <= 0);
	}

private:
	int32 OutstandingShots = 0;
	bool bDoneFiring = false;
	
};