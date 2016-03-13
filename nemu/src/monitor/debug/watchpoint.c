#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_list[NR_WP];
static WP *head, *free_;

void init_wp_list() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_list[i].NO = i;
		wp_list[i].swadd = 0;
		wp_list[i].next = &wp_list[i + 1];
	}
	wp_list[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_list;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp(){
	if(free == NULL){
		Assert(0, "No space for watchpoint\n");
	}
	if(head == NULL){
		head = free_;
		free_ = free_->next;
		head->next = NULL;
	}
	else{
		WP* new_wp = free_;
		free_ = new_wp->next;
		new_wp->next = NULL;
		WP* end = head;
		while(end->next != NULL){
			end = end->next;
		}
		end->next = new_wp;
	}
	return head;
}
void free_WP(WP* wp){
	if(head == wp){
		head = head->next;
		WP* f = free_;
		while(f->next != NULL){
			f = f->next;
		}
		wp->NO = f->NO+1;
		f->next = wp;
		wp->next = NULL;
	}
	else{
		WP* pre = head;
		while(pre->next != wp && pre->next != NULL){
			pre = pre->next;
		}
		if(pre->next == NULL){
			Assert(0, "No such watchpoint\n");
		}
		pre->next = wp->next;
		WP* f = free_;
		while(f->next != NULL){
			f = f->next;
		}
		wp->NO = f->NO+1;
		f->next = wp;
		wp->next = NULL;
	}
}
