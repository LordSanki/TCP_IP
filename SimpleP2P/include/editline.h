/* Minix editline */
#ifndef __EDITLINE_H__
#define __EDITLINE_H__

/*
**  For compatibility with FSF readline.
*/

#define EXTERN extern "C"

EXTERN void rl_reset_terminal(char *p);
EXTERN void rl_initialize(void);

EXTERN char *readline(const char *prompt);
EXTERN void add_history(char *line); /* OBSOLETE: Made part of readline(). -- kjb */

#endif  /* __EDITLINE_H__ */
