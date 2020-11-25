//-------------------------------------------------
// #### DEXEL 6CH BIDIRECTIONAL - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### VERSION.H #################################
//-------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _VERSION_H_
#define _VERSION_H_


// Exported Constants ----------------------------------------------------------
#define HARDWARE_VERSION_2_2    //2020 prototypes 4 units, & production boards 112 units
// #define HARDWARE_VERSION_2_0    //2020 prototypes 4 units
// #define HARDWARE_VERSION_1_0    //2018 prototypes 4 units


#define SOFTWARE_VERSION_1_0
// #define SOFTWARE_VERSION_1_1


//--- Hardware Welcome Code ------------------//
#ifdef HARDWARE_VERSION_2_2
#define HARD "Hardware Version: 2.2\n"
#define HARD_VER "2.2"
#endif
#ifdef HARDWARE_VERSION_2_0
#define HARD "Hardware Version: 2.0\n"
#define HARD_VER "2.0"
#endif
#ifdef HARDWARE_VERSION_1_0
#define HARD "Hardware Version: 1.0\n"
#define HARD_VER "1.0"
#endif

//--- Software Welcome Code ------------------//
#ifdef SOFTWARE_VERSION_1_2
#define SOFT "Software Version: 1.2\n"
#define SOFT_VER "1.2"
#endif
#ifdef SOFTWARE_VERSION_1_1
#define SOFT "Software Version: 1.1\n"
#define SOFT_VER "1.1"
#endif
#ifdef SOFTWARE_VERSION_1_0
#define SOFT "Software Version: 1.0\n"
#define SOFT_VER "1.0"
#endif


#endif    /* _VERSION_H_ */

//--- end of file ---//
