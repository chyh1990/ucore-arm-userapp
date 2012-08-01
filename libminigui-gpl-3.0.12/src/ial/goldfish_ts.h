/*
 ** goldfish_ts.h:. the head file of Low Level Input Engine for GOLDFISH
 **
 **
 */

#ifndef GUI_IAL_GOLDFISH_H
    #define GUI_IAL_GOLDFISH_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    BOOL    InitGOLDFISHInput (INPUT* input, const char* mdev, const char* mtype); 
    void    TermGOLDFISHInput (void); 

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_FiguerOA_H */

