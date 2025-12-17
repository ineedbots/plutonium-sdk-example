#pragma once

class component_interface
{
public:
	virtual ~component_interface()
	{
	}

	virtual void on_startup()
	{
	}

	virtual void on_shutdown()
	{
	}

	virtual bool is_supported()
	{
		return true;
	}
};
