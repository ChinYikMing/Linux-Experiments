#include "list.h"
#include <assert.h>
#include <stdlib.h>

struct data {
	int val;
	struct list_head list;
};

struct data *data_new(int val){
	struct data *data = malloc(sizeof(struct data));
	if(!data)
		return NULL;

	data->val = val;
	return data;
}

int main(){
	struct list_head list = LIST_HEAD_INIT(list);

	struct data *data = NULL;
	for(int i = 1; i <= 100; i++){
		data = data_new(i);
		assert(data != NULL);

		list_add(&data->list, &list);
	}

	struct list_head *iter;
	list_for_each(iter, &list){
		data = list_entry(iter, struct data, list);
		printf("data->val: %d\n", data->val);
	}
}
