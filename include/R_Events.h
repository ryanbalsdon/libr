#ifndef R_Events_h
#define R_Events_h

/* Author: Ryan Balsdon <ryanbalsdon@gmail.com>
 *  I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#include "R_Type.h"

typedef struct R_Events R_Events;
R_Type_Declare(R_Events);

/* R_Events_Callback
   The callback format used to register for events. Target is the registering/receiving object and payload is the message details.
 */
typedef void (*R_Events_Callback)(void* target, const char* event_key, void* payload);

/* R_Events_Register
   Register an event handler.
 */
R_Events* R_FUNCTION_ATTRIBUTES R_Events_register(R_Events* self, const char* event_key, void* target, R_Events_Callback callback);

/* R_Events_RegisterOnce
   Register an event handler that will only be called once.
 */
R_Events* R_FUNCTION_ATTRIBUTES R_Events_registerOnce(R_Events* self, const char* event_key, void* target, R_Events_Callback callback);

/* R_Events_Remove
   Remove an event handler.
 */
R_Events* R_FUNCTION_ATTRIBUTES R_Events_remove(R_Events* self, const char* event_key, const void* target, R_Events_Callback callback);

/* R_Events_RemoveTarget
   Removes all events registered by the given target. Useful for object destructors.
 */
R_Events* R_FUNCTION_ATTRIBUTES R_Events_removeTarget(R_Events* self, const void* target);

/* R_Events_Notify
   Send an event notification to all registered receivers.
 */
R_Events* R_FUNCTION_ATTRIBUTES R_Events_notify(R_Events* self, const char* event_key, void* payload);

/* R_Events_IsRegistered
   Returns true if the event handler has been registered or false if not. Returns false on error.
 */
bool R_FUNCTION_ATTRIBUTES R_Events_isRegistered(R_Events* self, const char* event_key, void* target, R_Events_Callback callback);


#endif /* R_Events_h */
