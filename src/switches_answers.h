//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### SWITCHES_ANSWERS.H ####################
//---------------------------------------------

#ifndef _SWITCHES_ANSWERS_H_
#define _SWITCHES_ANSWERS_H_

// Exported Types Constants and Macros -----------------------------------------

// Switches actions
typedef enum {
    do_nothing = 0,
    selection_up,
    selection_dwn,
    selection_enter,
    selection_back

} sw_actions_t;


typedef enum {
    resp_ok = 0,
    resp_continue,
    resp_selected,
    resp_change,
    resp_change_all_up,
    resp_working,
    resp_error,
    resp_need_to_save,
    resp_finish,
    //main menu
    resp_save

} resp_t;


#endif /* _SWITCHES_ANSWERS_H_ */

//--- end of file ---//
