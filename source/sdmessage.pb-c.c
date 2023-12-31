/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: sdmessage.proto */

/* Do not generate deprecated warnings for self */
#ifndef PROTOBUF_C__NO_DEPRECATED
#define PROTOBUF_C__NO_DEPRECATED
#endif

#include "sdmessage.pb-c.h"
void   message__t__init
                     (MessageT         *message)
{
  static const MessageT init_value = MESSAGE__T__INIT;
  *message = init_value;
}
size_t message__t__get_packed_size
                     (const MessageT *message)
{
  assert(message->base.descriptor == &message__t__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t message__t__pack
                     (const MessageT *message,
                      uint8_t       *out)
{
  assert(message->base.descriptor == &message__t__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t message__t__pack_to_buffer
                     (const MessageT *message,
                      ProtobufCBuffer *buffer)
{
  assert(message->base.descriptor == &message__t__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
MessageT *
       message__t__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (MessageT *)
     protobuf_c_message_unpack (&message__t__descriptor,
                                allocator, len, data);
}
void   message__t__free_unpacked
                     (MessageT *message,
                      ProtobufCAllocator *allocator)
{
  if(!message)
    return;
  assert(message->base.descriptor == &message__t__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
static const ProtobufCEnumValue message__t__opcode__enum_values_by_number[9] =
{
  { "OP_BAD", "MESSAGE__T__OPCODE__OP_BAD", 0 },
  { "OP_SIZE", "MESSAGE__T__OPCODE__OP_SIZE", 10 },
  { "OP_DEL", "MESSAGE__T__OPCODE__OP_DEL", 20 },
  { "OP_GET", "MESSAGE__T__OPCODE__OP_GET", 30 },
  { "OP_PUT", "MESSAGE__T__OPCODE__OP_PUT", 40 },
  { "OP_GETKEYS", "MESSAGE__T__OPCODE__OP_GETKEYS", 50 },
  { "OP_PRINT", "MESSAGE__T__OPCODE__OP_PRINT", 60 },
  { "OP_STATS", "MESSAGE__T__OPCODE__OP_STATS", 70 },
  { "OP_ERROR", "MESSAGE__T__OPCODE__OP_ERROR", 99 },
};
static const ProtobufCIntRange message__t__opcode__value_ranges[] = {
{0, 0},{10, 1},{20, 2},{30, 3},{40, 4},{50, 5},{60, 6},{70, 7},{99, 8},{0, 9}
};
static const ProtobufCEnumValueIndex message__t__opcode__enum_values_by_name[9] =
{
  { "OP_BAD", 0 },
  { "OP_DEL", 2 },
  { "OP_ERROR", 8 },
  { "OP_GET", 3 },
  { "OP_GETKEYS", 5 },
  { "OP_PRINT", 6 },
  { "OP_PUT", 4 },
  { "OP_SIZE", 1 },
  { "OP_STATS", 7 },
};
const ProtobufCEnumDescriptor message__t__opcode__descriptor =
{
  PROTOBUF_C__ENUM_DESCRIPTOR_MAGIC,
  "message__t.Opcode",
  "Opcode",
  "MessageT__Opcode",
  "",
  9,
  message__t__opcode__enum_values_by_number,
  9,
  message__t__opcode__enum_values_by_name,
  9,
  message__t__opcode__value_ranges,
  NULL,NULL,NULL,NULL   /* reserved[1234] */
};
static const ProtobufCEnumValue message__t__c_type__enum_values_by_number[8] =
{
  { "CT_BAD", "MESSAGE__T__C_TYPE__CT_BAD", 0 },
  { "CT_KEY", "MESSAGE__T__C_TYPE__CT_KEY", 10 },
  { "CT_VALUE", "MESSAGE__T__C_TYPE__CT_VALUE", 20 },
  { "CT_ENTRY", "MESSAGE__T__C_TYPE__CT_ENTRY", 30 },
  { "CT_KEYS", "MESSAGE__T__C_TYPE__CT_KEYS", 40 },
  { "CT_RESULT", "MESSAGE__T__C_TYPE__CT_RESULT", 50 },
  { "CT_TABLE", "MESSAGE__T__C_TYPE__CT_TABLE", 60 },
  { "CT_NONE", "MESSAGE__T__C_TYPE__CT_NONE", 70 },
};
static const ProtobufCIntRange message__t__c_type__value_ranges[] = {
{0, 0},{10, 1},{20, 2},{30, 3},{40, 4},{50, 5},{60, 6},{70, 7},{0, 8}
};
static const ProtobufCEnumValueIndex message__t__c_type__enum_values_by_name[8] =
{
  { "CT_BAD", 0 },
  { "CT_ENTRY", 3 },
  { "CT_KEY", 1 },
  { "CT_KEYS", 4 },
  { "CT_NONE", 7 },
  { "CT_RESULT", 5 },
  { "CT_TABLE", 6 },
  { "CT_VALUE", 2 },
};
const ProtobufCEnumDescriptor message__t__c_type__descriptor =
{
  PROTOBUF_C__ENUM_DESCRIPTOR_MAGIC,
  "message__t.C_type",
  "C_type",
  "MessageT__CType",
  "",
  8,
  message__t__c_type__enum_values_by_number,
  8,
  message__t__c_type__enum_values_by_name,
  8,
  message__t__c_type__value_ranges,
  NULL,NULL,NULL,NULL   /* reserved[1234] */
};
static const ProtobufCFieldDescriptor message__t__field_descriptors[13] =
{
  {
    "opcode",
    1,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_ENUM,
    0,   /* quantifier_offset */
    offsetof(MessageT, opcode),
    &message__t__opcode__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "c_type",
    2,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_ENUM,
    0,   /* quantifier_offset */
    offsetof(MessageT, c_type),
    &message__t__c_type__descriptor,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "size",
    3,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_SINT32,
    0,   /* quantifier_offset */
    offsetof(MessageT, size),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "data",
    4,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_BYTES,
    0,   /* quantifier_offset */
    offsetof(MessageT, data),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "key",
    5,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    offsetof(MessageT, key),
    NULL,
    &protobuf_c_empty_string,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "info",
    6,
    PROTOBUF_C_LABEL_REPEATED,
    PROTOBUF_C_TYPE_STRING,
    offsetof(MessageT, n_info),
    offsetof(MessageT, info),
    NULL,
    &protobuf_c_empty_string,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "n_times_SIZE",
    7,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_SINT32,
    0,   /* quantifier_offset */
    offsetof(MessageT, n_times_size),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "n_times_DEL",
    8,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_SINT32,
    0,   /* quantifier_offset */
    offsetof(MessageT, n_times_del),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "n_times_GET",
    9,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_SINT32,
    0,   /* quantifier_offset */
    offsetof(MessageT, n_times_get),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "n_times_PUT",
    10,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_SINT32,
    0,   /* quantifier_offset */
    offsetof(MessageT, n_times_put),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "n_times_GETKEYS",
    11,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_SINT32,
    0,   /* quantifier_offset */
    offsetof(MessageT, n_times_getkeys),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "n_times_PRINT",
    12,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_SINT32,
    0,   /* quantifier_offset */
    offsetof(MessageT, n_times_print),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
  {
    "avg_time",
    13,
    PROTOBUF_C_LABEL_NONE,
    PROTOBUF_C_TYPE_DOUBLE,
    0,   /* quantifier_offset */
    offsetof(MessageT, avg_time),
    NULL,
    NULL,
    0,             /* flags */
    0,NULL,NULL    /* reserved1,reserved2, etc */
  },
};
static const unsigned message__t__field_indices_by_name[] = {
  12,   /* field[12] = avg_time */
  1,   /* field[1] = c_type */
  3,   /* field[3] = data */
  5,   /* field[5] = info */
  4,   /* field[4] = key */
  7,   /* field[7] = n_times_DEL */
  8,   /* field[8] = n_times_GET */
  10,   /* field[10] = n_times_GETKEYS */
  11,   /* field[11] = n_times_PRINT */
  9,   /* field[9] = n_times_PUT */
  6,   /* field[6] = n_times_SIZE */
  0,   /* field[0] = opcode */
  2,   /* field[2] = size */
};
static const ProtobufCIntRange message__t__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 13 }
};
const ProtobufCMessageDescriptor message__t__descriptor =
{
  PROTOBUF_C__MESSAGE_DESCRIPTOR_MAGIC,
  "message__t",
  "MessageT",
  "MessageT",
  "",
  sizeof(MessageT),
  13,
  message__t__field_descriptors,
  message__t__field_indices_by_name,
  1,  message__t__number_ranges,
  (ProtobufCMessageInit) message__t__init,
  NULL,NULL,NULL    /* reserved[123] */
};
