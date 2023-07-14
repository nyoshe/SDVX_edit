#pragma once
#ifndef FSMLIST_HPP_INCLUDED
#define FSMLIST_HPP_INCLUDED

#include "tinyfsm.hpp"

#include "EditWindow.h"

using fsm_list = tinyfsm::FsmList<EditController>;

/** dispatch event to both "Motor" and "Elevator" */
template<typename E>
void send_event(E const& event)
{
	fsm_list::template dispatch<E>(event);
}

#endif