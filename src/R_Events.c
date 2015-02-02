/* Author: Ryan Balsdon <ryanbalsdon@gmail.com>
 *  I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "R_Events.h"
#include "R_List.h"
#include "R_Dictionary.h"



struct R_Events {
	R_Type* type;
	R_Dictionary* events;
};
static R_Events* R_Events_Constructor(R_Events* self);
static R_Events* R_Events_Destructor(R_Events* self);
R_Type_Def(R_Events, R_Events_Constructor, R_Events_Destructor, NULL);

typedef struct {
	R_Type* type;
	void* target;
	R_Events_Callback callback;
	bool run_once;
} R_Events_NotificationDefinition;
R_Type_Def(R_Events_NotificationDefinition, NULL, NULL, NULL);

static R_Events* R_Events_Constructor(R_Events* self) {
	self->events = R_Type_New(R_Dictionary);
	if (self->events == NULL) return NULL;
	return self;
}

static R_Events* R_Events_Destructor(R_Events* self) {
	R_Type_Delete(self->events);
	return self;
}

static R_Events_NotificationDefinition* R_Events_newDefinition(R_Events* self, const char* event_key);
R_Events* R_Events_register(R_Events* self, const char* event_key, void* target, R_Events_Callback callback) {
	if (self == NULL || event_key == NULL || callback == NULL) return NULL; //target can be NULL!
	R_Events_NotificationDefinition* def = R_Events_newDefinition(self, event_key);
	if (def == NULL) return NULL;
	def->callback = callback;
	def->target = target;
	def->run_once = false;
	return self;
}

R_Events* R_Events_registerOnce(R_Events* self, const char* event_key, void* target, R_Events_Callback callback) {
	if (self == NULL || event_key == NULL || callback == NULL) return NULL; //target can be NULL!
	R_Events_NotificationDefinition* def = R_Events_newDefinition(self, event_key);
	if (def == NULL) return NULL;
	def->callback = callback;
	def->target = target;
	def->run_once = true;
	return self;
}

static R_Events_NotificationDefinition* R_Events_newDefinition(R_Events* self, const char* event_key) {
	if (self == NULL || event_key == NULL) return NULL;

	R_List* events_for_key = NULL;
	if (R_Dictionary_isNotPresent(self->events, event_key)) 
		events_for_key = R_Dictionary_add(self->events, event_key, R_List);
	else events_for_key = R_Dictionary_get(self->events, event_key);
	if (events_for_key == NULL) return NULL;

	R_Events_NotificationDefinition* def = R_List_add(events_for_key, R_Events_NotificationDefinition);
	if (def == NULL) return NULL;
	return def;
}

R_Events* R_Events_remove(R_Events* self, const char* event_key, const void* target, R_Events_Callback callback) {
	if (self == NULL || event_key == NULL || callback == NULL) return NULL; //target can be NULL!
	R_List* events_for_key = R_Dictionary_get(self->events, event_key);
	if (events_for_key == NULL) return NULL;

	R_Functor* iterator = R_List_Iterator(R_Type_New(R_Functor), events_for_key);
	if (iterator == NULL) return NULL;
	R_Events_NotificationDefinition* def = NULL;
	while ((def = R_Functor_call(iterator)) != NULL) {
		if (def->target == target && def->callback == callback) R_List_removePointer(events_for_key, def);
	}
	R_Type_Delete(iterator);
	return self;
}

R_Events* R_Events_removeTarget(R_Events* self, const void* target) {
	if (self == NULL) return NULL; //target can be NULL!

	R_Functor* event_iterator = R_Dictionary_ValueIterator(R_Type_New(R_Functor), self->events);
	if (event_iterator == NULL) return NULL;
	R_List* event = NULL;
	while ((event = R_Functor_call(event_iterator)) != NULL) {
		R_Functor* def_iterator = R_List_Iterator(R_Type_New(R_Functor), event);
		if (def_iterator == NULL) return R_Type_Delete(event_iterator), NULL;
		R_Events_NotificationDefinition* def = NULL;
		while ((def = R_Functor_call(def_iterator)) != NULL) {
			if (def->target == target) R_List_removePointer(event, def);
		}
		R_Type_Delete(def_iterator);
	}
	R_Type_Delete(event_iterator);

	return self;
}

R_Events* R_Events_notify(R_Events* self, const char* event_key, void* payload) {
	if (self == NULL || event_key == NULL) return NULL; //payload can be NULL!
	R_List* events_for_key = R_Dictionary_get(self->events, event_key);
	if (events_for_key == NULL) return NULL;

	R_Functor* iterator = R_List_Iterator(R_Type_New(R_Functor), events_for_key);
	if (iterator == NULL) return NULL;
	R_Events_NotificationDefinition* def = NULL;
	while ((def = R_Functor_call(iterator)) != NULL) {
		if (def->callback) def->callback(def->target, event_key, payload);
		if (def->run_once) R_List_removePointer(events_for_key, def);
	}
	R_Type_Delete(iterator);
	return self;
}

bool R_Events_isRegistered(R_Events* self, const char* event_key, void* target, R_Events_Callback callback) {
	if (self == NULL || event_key == NULL || callback == NULL) return false;
	R_List* events_for_key = R_Dictionary_get(self->events, event_key);
	if (events_for_key == NULL) return false;
	R_Functor* iterator = R_List_Iterator(R_Type_New(R_Functor), events_for_key);
	if (iterator == NULL) return false;
	R_Events_NotificationDefinition* def = NULL;
	while ((def = R_Functor_call(iterator)) != NULL) {
		if (def->target == target && def->callback == callback) return R_Type_Delete(iterator), true;
	}
	R_Type_Delete(iterator);
	return false;
}

