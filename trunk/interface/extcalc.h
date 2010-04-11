#ifndef EXTCALC_H_
#define EXTCALC_H_

#ifdef __cpluscplus
extern "C" {
#endif
 
#ifndef HAS_CALC_H
typedef void calc_t;
#endif

calc_t *calc_new(void);
void calc_load(calc_t *, const char *);
void calc_key_press(calc_t *,int keycode);
void calc_key_release(calc_t *,int keycode);
uint8_t* calc_draw(calc_t *calc);
 
#ifdef __cpluscplus
}
#endif

#endif /*EXTCALC_H_*/
