// Actions.h

// maintain a list of knows actions from the current active elements.

// A name in this collection can be comapred by the address of the string in the set() function of the element.
// The board class converts any given name by (char *) or String into the (const char *) of the lowercase equivalent of this set of action names.



namespace HomeDing {

namespace Action {

// find the action name in the Action collection or return null.
const char *find(const char *name);

// allow direct pointer compare for these actions:
extern const char *Address;
extern const char *Border;
extern const char *Down;
extern const char *Height;
extern const char *Label;
extern const char *Max;
extern const char *Min;
extern const char *OnHigh;
extern const char *OnLow;
extern const char *OnValue;
extern const char *Pin;
extern const char *Step;
extern const char *Text;
extern const char *Toggle;
extern const char *Type;
extern const char *Up;
extern const char *Value;
extern const char *Width;

}; // namespace Action

}; // namespace Homeding

// End