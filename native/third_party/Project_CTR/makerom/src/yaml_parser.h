#pragma once

#include <libyaml/yaml.h>

typedef enum
{
	YAML_API_ERROR = -1,
	YAML_BAD_GROUP_HEADER = -2,
	YAML_BAD_FORMATTING = -3,
	YAML_MEM_ERROR = -4,
	YAML_UNKNOWN_KEY = -5,
} ctr_yaml_error;

typedef struct
{
	// For Continued Parsing of file
	yaml_parser_t parser;
	yaml_event_t event;
	bool done;
	int error;
	
	// Important Details
	dname_struct *dname;
	bool IsSequence;
	bool IsKey;
	yaml_event_type_t prev_event;
	u32 Level;
	
	// Processed String
	char *string;
} ctr_yaml_context;

// Public Prototypes
int GetRsfSettings(user_settings *set);

// For scalar events
char *GetYamlString(ctr_yaml_context *ctx);
u32 GetYamlStringSize(ctr_yaml_context *ctx);
bool cmpYamlValue(char *string,ctr_yaml_context *ctx); // Compares a string to the current scalar event
bool casecmpYamlValue(char *string,ctr_yaml_context *ctx); // same as above but ignores case

// Event Handlers
void GetEvent(ctr_yaml_context *ctx);
void FinishEvent(ctr_yaml_context *ctx);


// Event Type Checks
bool EventIsScalar(ctr_yaml_context *ctx);
bool EventIsMappingStart(ctr_yaml_context *ctx);
bool EventIsMappingEnd(ctr_yaml_context *ctx);
bool EventIsSequenceStart(ctr_yaml_context *ctx);
bool EventIsSequenceEnd(ctr_yaml_context *ctx);
bool CheckMappingEvent(ctr_yaml_context *ctx); // With extra implement, use if lazy
bool CheckSequenceEvent(ctr_yaml_context *ctx); // With extra implement, use if lazy


// Functions which store values
void SetSimpleYAMLValue(char **dest, char *key, ctr_yaml_context *ctx, u32 size_limit);
void SetBoolYAMLValue(bool *dest, char *key, ctr_yaml_context *ctx);
u32 SetYAMLSequence(char ***dest, char *key, ctr_yaml_context *ctx);
u32 SetYAMLSequenceFromMapping(char ***dest, char *key, ctr_yaml_context *ctx, bool StoreKey);
//void SkipYAMLGroup(ctr_yaml_context *ctx);