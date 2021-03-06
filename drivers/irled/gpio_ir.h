/*
  * Samsung Mobile VE Group.
  *
  * drivers/irled/gpio_ir.h
  *
  * Header for samsung gpio irled.
  *
  * Copyright (c) 2014 Samsung Electronics Co., Ltd.
  *  Jeeon Park <jeeon.park@samsung.com>
  *
  */

#ifndef _GPIO_IR_H_
#define _GPIO_IR_H_

/*************************************************/
/* If need, define this feature and make related code
     in accordance with the state of each BB platform.

     for example : Exynos BB use spinlock of gpio pin bank
                           when setting gpio output value.
                           Make custom gpio setting func. (no spinlock),
                           and use spinlock of gpio pin (overall area)
                           during area of ir data
                           and use custom gpio func. for gpio setting. */
/*#define GPIO_IR_CUSTOM_SPINLOCK */
/*************************************************/


#ifdef CONFIG_ARM_ARCH_TIMER
#define GPIO_IR_MULTI_TIMER
#define GPIO_IR_MULTI_TIMER_PRECALL
#endif

/***********************************************/
/* These values may be changed in each models */
/***********************************************/
#define IR_TIMER_CALL_DELAY	29670
#define TIMER_TYPE2_COND_TIME	1000000

#define GIR_DATA_START_DELAY	60000000
#define GIR_DATA_END_DELAY		13000000

/* If device did not use dts, This value is used. */
#ifndef CONFIG_OF
#define GPIO_IR_LED_EN	301
#define GPIO_IRLED_PIN	85
#endif

#define IR_GPIO_HIGH_DELAY	400
#define IR_GPIO_LOW_DELAY	100
/***********************************************/

#define	NO_PIN_DETECTED		-1

#define GPIO_LEVEL_LOW	0
#define GPIO_LEVEL_HIGH	1


#define IR_DATA_SIZE	1024
#define IR_FREQ_UNIT_MHZ		1000000UL
#define IR_FREQ_UNIT_GHZ		1000000000UL


#define GIR_MALLOC(NAME, SIZE)								\
	do {														\
		if (!NAME) {											\
			NAME = kzalloc((SIZE), GFP_KERNEL);					\
			if (!(NAME)) {										\
				pr_err("Fail to allocate " #NAME " arrays!!\n");		\
				ret = -ENOMEM;								\
				goto error;									\
			}												\
		} else {												\
			memset((NAME), 0, SIZE);							\
		}													\
	} while (0)

#define GIR_FREE(NAME)						\
	do {										\
		if ((NAME)) {							\
			kfree((NAME));					\
			NAME = NULL;						\
		}									\
	} while (0)


struct gpio_ir_timer_info_t
{
	int pos;
	bool bstart;
	bool bend;
	struct hrtimer timer;
	struct gpio_ir_info_t *info;
};

struct gpio_ir_info_t
{
	/******************************************/
	/* Only Once Init Variables */
	/******************************************/
	int gpio;
	int en_gpio;
	unsigned long gpio_high_delay;
	unsigned long gpio_low_delay;
#ifdef GPIO_IR_MULTI_TIMER
	struct hrtimer timer_end;
#endif
 	struct hrtimer memfree_timer;
	struct work_struct post_run_work;
	struct completion ir_send_comp;
	unsigned long start_delay;
	unsigned long end_delay;
	long timer_delay;
	struct pm_qos_request pm_qos_req;
	long qos_delay;

	unsigned long partial_timer_type;
	bool blow_data_correction;		// low data length correction

	/******************************************/
	/* to be initialized data when IR starts */
	/******************************************/
	unsigned int *data;

#ifdef GPIO_IR_MULTI_TIMER
	struct gpio_ir_timer_info_t tinfo[NR_CPUS];
	int *cur_sound_cpu;
	int *cur_tsp_cpu;
	atomic_t timer_count;
	cycles_t start_cycle;
	unsigned long start_delay_cycles;
	cycles_t hm_start_cycle;
	ktime_t hm_start_time;
	unsigned long *delay_cycles;
	unsigned long *timer_type2_next;
	bool bvalid_timer_run;
	bool hm_start_time_save;
	bool bir_init_comp;
#else
	struct hrtimer ir_timer;
#endif

	int data_pos;
	unsigned long *delay;
	ktime_t *ktime_delay;
	unsigned long *data_count;

	bool bcompleted;
	bool check_ir_send;

	/******************************************/
	/* Only Once and specific time modified data */
	/******************************************/
	bool brun;
	bool bir_policy_set;
	unsigned long ir_data_type;

	/******************************************/
	/* Always calculated temp data */
	/******************************************/
	unsigned int freq;
	int count;
	unsigned long up_delay;
	unsigned long down_delay;
#ifdef GPIO_IR_MULTI_TIMER
	unsigned long up_delay_cycles;
	unsigned long down_delay_cycles;
#endif
	unsigned long period;

};

#ifndef GPIO_IR_MULTI_TIMER
extern unsigned int __loop_custom_ndelay(unsigned long nsecs);
#endif

extern int hrtimer_start_gpio_ir(struct hrtimer *timer, ktime_t tim,
		unsigned long delta_ns, const enum hrtimer_mode mode,
		int wakeup);

extern void hrtimer_init_gpio_ir(struct hrtimer *timer, clockid_t clock_id,
		  enum hrtimer_mode mode, int cpu);

#ifdef CONFIG_SMP
extern int gpio_ir_cpu_up(void);		// all possible cpus up
#endif
#endif
