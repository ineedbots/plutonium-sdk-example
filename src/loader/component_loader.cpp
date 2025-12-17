#include <stdinc.hpp>
#include "component_loader.hpp"
#include "component_interface.hpp"

void component_loader::register_component(std::unique_ptr<component_interface>&& component_)
{
	get_components().push_back(std::move(component_));
}

void component_loader::on_startup()
{
	for (const auto& component_ : get_components())
	{
		if (component_->is_supported())
		{
			component_->on_startup();
		}
	}
}

void component_loader::on_shutdown()
{
	for (const auto& component_ : get_components())
	{
		if (component_->is_supported())
		{
			component_->on_shutdown();
		}
	}

	get_components().clear();
}

std::vector<std::unique_ptr<component_interface>>& component_loader::get_components()
{
	static std::vector<std::unique_ptr<component_interface>> components{};
	return components;
}
