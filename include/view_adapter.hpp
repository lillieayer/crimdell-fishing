// Handles logic to render translations, if necessary
#ifndef VIEW_ADAPTER_HPP
#define VIEW_ADAPTER_HPP

namespace UserInput
{
    // separates instantaneous key actions from game actions
    enum class Action 
    {
        START,
        QUIT,
        MOVEUP,
        MOVEDOWN,
        RELEASE,
        NONE
    };

}
#endif // VIEW_ADAPTER_HPP