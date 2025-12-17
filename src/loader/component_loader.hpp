#pragma once

class component_loader final
{
public:
	template <typename T>
	class installer final
	{
		static_assert(std::is_base_of<component_interface, T>::value, "component has invalid base class");

	public:
		installer()
		{
			register_component(std::make_unique<T>());
		}
	};

	template <typename T>
	static T* get()
	{
		for (const auto& component_ : get_components())
		{
			if (typeid(*component_.get()) == typeid(T))
			{
				return reinterpret_cast<T*>(component_.get());
			}
		}

		return nullptr;
	}

	static void register_component(std::unique_ptr<component_interface>&& component);

	static void on_startup();
	static void on_shutdown();

private:
	static std::vector<std::unique_ptr<component_interface>>& get_components();
};

#define REGISTER_COMPONENT(name)                          \
namespace                                                 \
{                                                         \
	static component_loader::installer<name> __component; \
}
