#include "lib.h"
#include "rsf_settings.h"

// Private Prototypes
void InitYamlContext(ctr_yaml_context *ctx);
int ParseSpecFile(rsf_settings *set, char *path, dname_struct *dname);
void ProcessYamlString(ctr_yaml_context *ctx);
void CheckEvent(ctr_yaml_context *ctx);


void BadYamlFormatting(void);

// Code
int GetRsfSettings(user_settings *set)
{
	int ret = 0;
	if(set->common.rsfPath) {
		if(!AssertFile(set->common.rsfPath)) {
			fprintf(stderr,"[RSF ERROR] Failed to open %s\n",set->common.rsfPath);
			return FAILED_TO_OPEN_FILE;
		}
		ret = ParseSpecFile(&set->common.rsfSet,set->common.rsfPath, &set->dname);
	}
	return ret;
}

int ParseSpecFile(rsf_settings *set, char *path, dname_struct *dname)
{
	ctr_yaml_context *ctx = malloc(sizeof(ctr_yaml_context));
	InitYamlContext(ctx);

	/* Set Specfile Type */
	
	/* Create the Parser object. */
	yaml_parser_initialize(&ctx->parser);

	/* Set a file input. */
	FILE *input = fopen(path,"rb");
	yaml_parser_set_input_file(&ctx->parser, input);
	
	
	ctx->dname = dname;
	ctx->IsSequence = false;
	ctx->IsKey = true;
	ctx->prev_event = 0;
	ctx->Level = 0;
	
	
	/* Read the event sequence. */
	while (!ctx->done) {
		/* Get the next event. */
		GetEvent(ctx);
		if(ctx->error) goto error;
		
		/* Proccess Event */
		
		
		if(EventIsScalar(ctx)){
			EvaluateRSF(set,ctx);
			if(ctx->error) goto error;
			break;
		}
		/*
		if((ctx->event.type == YAML_SEQUENCE_START_EVENT|| ctx->event.type == YAML_MAPPING_START_EVENT) && ctx->prev_event == YAML_SCALAR_EVENT) printf(":\n");
		if(ctx->event.type == YAML_SCALAR_EVENT){
			if(ctx->IsSequence){
				printf(" - %s\n",ctx->event.data.scalar.value);
			}
			else{
				if(!ctx->IsKey) printf(": %s\n",ctx->event.data.scalar.value);
				else printf("%s",ctx->event.data.scalar.value);
			}			
		}
		*/
		
		/* Finish Event */
		FinishEvent(ctx);
		if(ctx->error) goto error;
	}

	/* Destroy the Parser object. */
	yaml_parser_delete(&ctx->parser);
	fclose(input);
	return 0;

	/* On error. */
	error:
	fprintf(stderr,"[RSF ERROR] Error Proccessing RSF file\n");
	
	/* Destroy the Parser object. */
	yaml_parser_delete(&ctx->parser);
	fclose(input);
	return ctx->error;
}

void InitYamlContext(ctr_yaml_context *ctx)
{
	memset(ctx,0,sizeof(ctr_yaml_context));
}

void ProcessYamlString(ctr_yaml_context *ctx)
{
	if(ctx->string)
	{
		free(ctx->string);
		ctx->string = NULL;
	}

	if(!ctx->event.data.scalar.value)
		return;
	
	char *rawStr = (char*)ctx->event.data.scalar.value;
	int rawStrLen = strlen(rawStr);
	int procStrLen = 0;

	char *subStart = NULL;
	char *subEnd = NULL;
	char *pos = rawStr;
	char *end = (rawStr+rawStrLen);
	while(pos < end)
	{
		// Find substution syntax in string
		subStart = strstr(pos,"$(");
		if(!subStart)
		{
			procStrLen += (end - pos);
			break;
		}

		// Check For errors
		if((end - subStart) <= 3) // Valid use of substitution syntax is not possible
		{
			ctx->error = true;
			return;
		}

		subEnd = strstr((subStart+2),")"); 

		if(!subEnd) // no closing bracket
		{
			ctx->error = true;
			return;
		}

		// Add length of string not accounted for
		procStrLen += (int)(subStart - pos);

		// Get Length Of substitution key
		char *subName = (subStart+2);
		int subNameLen = (int)((subEnd - subStart) - 2);

		// Add length of substitutiion value
		for(u32 i = 0; i < ctx->dname->u_items; i++){
			char *testSubName = ctx->dname->items[i].name;
			int testSubNameLen = strlen(testSubName);
			char *testSubValue = ctx->dname->items[i].value;
			int testSubValueLen = strlen(testSubValue);

			if(testSubNameLen != subNameLen)
				continue;
			if(strncmp(testSubName,subName,subNameLen) != 0)
				continue;

			procStrLen += testSubValueLen;
			break;
		}

		// Increment pos
		pos = (subEnd + 1);
	}
	
	// Allocate memory for processed string
	ctx->string = calloc(procStrLen+1,sizeof(char));
	char *procStr = ctx->string;

	pos = rawStr;
	end = (rawStr+rawStrLen);
	while(pos < end)
	{
		// Find substution syntax in string
		subStart = strstr(pos,"$(");
		if(!subStart)
		{
			strncat(procStr,pos,(end - pos));
			break;
		}

		// Check For errors
		if((end - subStart) <= 3) // Valid use of substitution syntax is not possible
		{
			ctx->error = true;
			return;
		}

		subEnd = strstr((subStart+2),")"); 

		if(!subEnd) // no closing bracket
		{
			ctx->error = true;
			return;
		}

		// Add length of string not accounted for
		strncat(procStr,pos,(subStart - pos));

		// Get Length Of substitution key
		char *subName = (subStart+2);
		int subNameLen = (int)((subEnd - subStart) - 2);

		// Add length of substitutiion value
		for(u32 i = 0; i < ctx->dname->u_items; i++){
			char *testSubName = ctx->dname->items[i].name;
			int testSubNameLen = strlen(testSubName);
			char *testSubValue = ctx->dname->items[i].value;
			int testSubValueLen = strlen(testSubValue);

			if(testSubNameLen != subNameLen)
				continue;
			if(strncmp(testSubName,subName,subNameLen) != 0)
				continue;

			strncat(procStr,testSubValue,testSubValueLen);
			break;
		}

		// Increment pos
		pos = (subEnd + 1);
	}

	return;
}

char *GetYamlString(ctr_yaml_context *ctx)
{
	return ctx->string;
}


u32 GetYamlStringSize(ctr_yaml_context *ctx)
{
	return strlen(GetYamlString(ctx)); // can't read size from yaml, as string may have been intercepted
}

void GetEvent(ctr_yaml_context *ctx)
{
	if (!yaml_parser_parse(&ctx->parser, &ctx->event)){
		ctx->error = YAML_API_ERROR;
		return;
	}
	CheckEvent(ctx);
}

void CheckEvent(ctr_yaml_context *ctx)
{
	switch(ctx->event.type){
		case YAML_SCALAR_EVENT:
			ProcessYamlString(ctx);
			break;
		case YAML_SEQUENCE_START_EVENT: 
			ctx->IsSequence = true;
			ctx->IsKey = true;
			ctx->Level++;
			//printf("[LEVEL] %d\n",ctx->Level);
			break;
		case YAML_SEQUENCE_END_EVENT: 
			ctx->IsSequence = false;
			ctx->IsKey = true;
			ctx->Level--;
			//printf("[LEVEL] %d\n",ctx->Level);
			break;
		case YAML_MAPPING_START_EVENT: 
			ctx->IsKey = true;
			ctx->Level++;
			//printf("[LEVEL] %d\n",ctx->Level);
			break;
		case YAML_MAPPING_END_EVENT: 
			ctx->IsKey = true;
			ctx->Level--;
			//printf("[LEVEL] %d\n",ctx->Level);
			break;
		case YAML_DOCUMENT_END_EVENT:
		case YAML_STREAM_END_EVENT:
			ctx->done = true;
			break;
		default: break;
	}
}

void FinishEvent(ctr_yaml_context *ctx)
{
	if(ctx->event.type == YAML_SCALAR_EVENT) {
		if(!ctx->IsSequence){
			ctx->IsKey = !ctx->IsKey;
			//if(!ctx->IsKey)ctx->IsKey = true;
			//else ctx->IsKey = false;
		}
		if(ctx->string){
			free(ctx->string);
			ctx->string = NULL;
		}
	}

	ctx->prev_event = ctx->event.type;
	yaml_event_delete(&ctx->event);
}

bool EventIsScalar(ctr_yaml_context *ctx)
{
	return (ctx->event.type == YAML_SCALAR_EVENT);
}

bool EventIsMappingStart(ctr_yaml_context *ctx)
{
	return (ctx->event.type == YAML_MAPPING_START_EVENT);
}

bool EventIsMappingEnd(ctr_yaml_context *ctx)
{
	return (ctx->event.type == YAML_MAPPING_END_EVENT);
}

bool EventIsSequenceStart(ctr_yaml_context *ctx)
{
	return (ctx->event.type == YAML_SEQUENCE_START_EVENT);
}

bool EventIsSequenceEnd(ctr_yaml_context *ctx)
{
	return (ctx->event.type == YAML_SEQUENCE_END_EVENT);
}

bool CheckSequenceEvent(ctr_yaml_context *ctx)
{
	GetEvent(ctx);
	if(!EventIsSequenceStart(ctx)){
		FinishEvent(ctx);
		//fprintf(stderr,"[-] Bad formatting in Spec file (Expected Sequence)\n");
		//ctx->error = YAML_BAD_FORMATTING;
		return false;
	}
	FinishEvent(ctx);
	return true;
}

bool CheckMappingEvent(ctr_yaml_context *ctx)
{
	GetEvent(ctx);
	if(!EventIsMappingStart(ctx)){
		FinishEvent(ctx);
		//fprintf(stderr,"[-] Bad formatting in Spec file (Expected Mapping)\n");
		//ctx->error = YAML_BAD_FORMATTING;
		return false;
	}
	FinishEvent(ctx);
	return true;
}

void BadYamlFormatting(void)
{
	fprintf(stderr,"[-] Bad formatting in Spec file\n");
}


bool cmpYamlValue(char *string,ctr_yaml_context *ctx)
{
	return (strcmp(GetYamlString(ctx),string) == 0);
}

bool casecmpYamlValue(char *string,ctr_yaml_context *ctx)
{
	return (strcasecmp(GetYamlString(ctx),string) == 0);
}

void SetSimpleYAMLValue(char **dest, char *key, ctr_yaml_context *ctx, u32 size_limit)
{
	if(*dest){
		fprintf(stderr,"[RSF ERROR] Item '%s' is already set\n",key);
		ctx->error = YAML_MEM_ERROR;
		return;
	}

	GetEvent(ctx);
	if(ctx->error || ctx->done) return;
	if(!EventIsScalar(ctx)){
		fprintf(stderr,"[RSF ERROR] '%s' requires a value\n",key);
		ctx->error = YAML_BAD_FORMATTING;
		return;
	}
	if(!GetYamlStringSize(ctx)) return;
	
	u32 size = GetYamlStringSize(ctx);
	if(size > size_limit && size_limit) size = size_limit;
	

	char *tmp = *dest;
	tmp = malloc(size+2);
	if(!tmp) {
		ctx->error = YAML_MEM_ERROR;
		return;
	}
	memset(tmp,0,size+2);
	memcpy(tmp,GetYamlString(ctx),size);	
	
	//printf("Setting %s to %s (size of %d)\n",key,GetYamlString(ctx),size);
	//printf("Check: %s & %x\n",tmp,tmp);
	*dest = tmp;
	
}

void SetBoolYAMLValue(bool *dest, char *key, ctr_yaml_context *ctx)
{
	GetEvent(ctx);
	if(ctx->error || ctx->done) return;
	if(!EventIsScalar(ctx)){
		fprintf(stderr,"[RSF ERROR] '%s' requires a value\n",key);
		ctx->error = YAML_BAD_FORMATTING;
		return;
	}
	if(!GetYamlStringSize(ctx)){
		fprintf(stderr,"[RSF ERROR] '%s' requires a value\n",key);
		ctx->error = YAML_BAD_FORMATTING;
		return;
	}
	
	if(casecmpYamlValue("true",ctx))
		*dest = true;
	else if(casecmpYamlValue("false",ctx))
		*dest = false;
	else{
		fprintf(stderr,"[RSF ERROR] Invalid '%s'\n",key);
		ctx->error = YAML_BAD_FORMATTING;
	}
	
	return;
	
}

u32 SetYAMLSequence(char ***dest, char *key, ctr_yaml_context *ctx)
{
	if(*dest){
		fprintf(stderr,"[RSF ERROR] %s already set\n",key);
		ctx->error = YAML_MEM_ERROR;
		return 0;
	}

	u32 ActualCount = 0;
	u32 SlotCount = 0;
	char **tmp = *dest;
	if(!CheckSequenceEvent(ctx)) return 0;
	SlotCount = 10;
	tmp = malloc((SlotCount+1)*sizeof(char*));
	if(!tmp){
		ctx->error = YAML_MEM_ERROR;
		return 0;
	}	
	memset(tmp,0,(SlotCount+1)*sizeof(char*));
	GetEvent(ctx);
	if(ctx->error || ctx->done) return 0;
	if(!EventIsScalar(ctx)){
		fprintf(stderr,"[RSF ERROR] '%s' requires a value\n",key);
		ctx->error = YAML_BAD_FORMATTING;
		return 0;
	}
	
	
	if(!GetYamlStringSize(ctx)) return 0;
	u32 InitLevel = ctx->Level;
	while(ctx->Level == InitLevel){
		if(ctx->error || ctx->done) return 0;
		tmp[ActualCount] = malloc(GetYamlStringSize(ctx)+1);
		memset(tmp[ActualCount],0,GetYamlStringSize(ctx)+1);
		memcpy(tmp[ActualCount],GetYamlString(ctx),GetYamlStringSize(ctx));
		ActualCount++;
		if(ActualCount >= SlotCount){ // if Exceeding Ptr capacity, expand buffer
			SlotCount = SlotCount*2;
			/*
			char **tmp1 = malloc((SlotCount+1)*sizeof(char*)); // allocate new buffer
			if(!tmp1){
				ctx->error = YAML_MEM_ERROR;
				return 0;
			}	
			memset(tmp1,0,(SlotCount+1)*sizeof(char*));
			for(u32 i = 0; i < ActualCount; i++) tmp1[i] = tmp[i]; // Transfer ptrs
			free(tmp); // free original buffer
			tmp = tmp1; // transfer main ptr
			*/
			tmp = realloc(tmp,(SlotCount+1)*sizeof(char*));
			if(!tmp){
				ctx->error = true;
				return 0;
			}
		}
		FinishEvent(ctx);
		GetEvent(ctx);
	}
	FinishEvent(ctx);
	*dest = tmp; // Give main ptr to location
	return ActualCount++; // return number of strings
}

u32 SetYAMLSequenceFromMapping(char ***dest, char *key, ctr_yaml_context *ctx, bool StoreKey)
{
	if(*dest){
		fprintf(stderr,"[RSF ERROR] %s already set\n",key);
		ctx->error = YAML_MEM_ERROR;
		return 0;
	}

	u32 ActualCount = 0;
	u32 SlotCount = 0;
	char **tmp = *dest;
	if(!CheckMappingEvent(ctx)) return 0;
	SlotCount = 10;
	tmp = malloc((SlotCount+1)*sizeof(char*));
	if(!tmp){
		ctx->error = YAML_MEM_ERROR;
		return 0;
	}	
	memset(tmp,0,(SlotCount+1)*sizeof(char*));
	GetEvent(ctx);
	if(ctx->error || ctx->done) return 0;
	if(!EventIsScalar(ctx)){
		fprintf(stderr,"[RSF ERROR] '%s' requires a value\n",key);
		ctx->error = YAML_BAD_FORMATTING;
		return 0;
	}
	
	
	if(!GetYamlStringSize(ctx)) return 0;
	u32 InitLevel = ctx->Level;
	while(ctx->Level == InitLevel){
		if(ctx->error || ctx->done) return 0;
		if(ctx->IsKey == StoreKey){
			tmp[ActualCount] = malloc(GetYamlStringSize(ctx)+1);
			memset(tmp[ActualCount],0,GetYamlStringSize(ctx)+1);
			memcpy(tmp[ActualCount],GetYamlString(ctx),GetYamlStringSize(ctx));
			ActualCount++;
			if(ActualCount >= SlotCount){ // if Exceeding Ptr capacity, expand buffer
				SlotCount = SlotCount*2;
				char **tmp1 = malloc((SlotCount+1)*sizeof(char*)); // allocate new buffer
				if(!tmp1){
					ctx->error = YAML_MEM_ERROR;
					return 0;
				}	
				memset(tmp1,0,(SlotCount+1)*sizeof(char*));
				for(u32 i = 0; i < ActualCount; i++) tmp1[i] = tmp[i]; // Transfer ptrs
				free(tmp); // free original buffer
				tmp = tmp1; // transfer main ptr
			}
		}
		FinishEvent(ctx);
		GetEvent(ctx);
	}
	FinishEvent(ctx);
	*dest = tmp; // Give main ptr to location
	return ActualCount++; // return number of strings
}

/*
void SkipYAMLGroup(ctr_yaml_context *ctx)
{
	FinishEvent(ctx);
	GetEvent(ctx);
	if(!EventIsMappingStart(ctx) && !EventIsSequenceStart(ctx) && EventIsScalar(ctx)) return;
	FinishEvent(ctx);
	GetEvent(ctx);
	
	if(ctx->error || ctx->done) return;
	if(!EventIsScalar(ctx)){
		fprintf(stderr,"[RSF ERROR] Format error\n");
		ctx->error = YAML_BAD_FORMATTING;
		return;
	}
	if(!GetYamlStringSize(ctx)) return;
	u32 InitLevel = ctx->Level;
	while(ctx->Level == InitLevel){
		if(ctx->error || ctx->done) return;
		FinishEvent(ctx);
		GetEvent(ctx);
	}
	FinishEvent(ctx);
}
*/