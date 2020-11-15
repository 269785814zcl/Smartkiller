#pragma once

#ifdef __cplusplus
extern "C" {
#endif
	
#define constrain(amt,low,high) \
((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

void AndroidTask(void);
void Emergency_stop(void);


#ifdef __cplusplus
}
#endif