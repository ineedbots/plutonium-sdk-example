#include <stdinc.hpp>
#include "component_loader.hpp"

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
	using component_vector = std::vector<std::unique_ptr<component_interface>>;
	using component_vector_container = std::unique_ptr<component_vector, std::function<void(component_vector*)>>;

	static component_vector_container components(new component_vector);
	return *components;
}
