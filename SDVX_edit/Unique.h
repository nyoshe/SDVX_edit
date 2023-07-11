#pragma once
template <typename T>
class Unique
{
public:
	static T& instance();

	Unique(const Unique&) = delete;
	Unique& operator=(Unique) = delete;
	Unique(Unique&&) = delete;
	Unique& operator=(Unique&&) = delete;

protected:
	struct token {};

	Unique() {}
};

template <typename T>
T& Unique<T>::instance()
{
	static T instance;
	return instance;
}
