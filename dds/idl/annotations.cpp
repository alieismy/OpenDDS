#include "annotations.h"

#include "be_extern.h"

#include <ast_annotation_decl.h>
#include <ast_annotation_appl.h>
#include <ast_annotation_member.h>
#include <ast_union.h>
#include <utl_string.h>

void Annotations::register_all()
{
  register_one<KeyAnnotation>();
  register_one<TopicAnnotation>();
  register_one<NestedAnnotation>();
  register_one<DefaultNestedAnnotation>();
  register_one<IdAnnotation>();
  register_one<AutoidAnnotation>();
  register_one<HashidAnnotation>();
  register_one<ExtensibilityAnnotation>();
  register_one<FinalAnnotation>();
  register_one<AppendableAnnotation>();
  register_one<MutableAnnotation>();
  register_one<OpenDDS::DataRepresentationAnnotation>();
}

Annotations::Annotations()
{
}

Annotations::~Annotations()
{
  for (MapType::iterator i = map_.begin(); i != map_.end(); ++i) {
    delete i->second;
  }
}

Annotation* Annotations::operator[](const std::string& annotation)
{
  return map_[annotation];
}

Annotation::Annotation()
: declaration_(0)
{
}

Annotation::~Annotation()
{
}

std::string Annotation::fullname() const
{
  return std::string("::@") + name();
}

AST_Annotation_Decl* Annotation::declaration() const
{
  return declaration_;
}

AST_Annotation_Appl* Annotation::find_on(AST_Decl* node) const
{
  return node->annotations().find(declaration_);
}

void Annotation::cache()
{
  idl_global->eval(definition().c_str());
  UTL_Scope* root = idl_global->scopes().bottom();
  declaration_ = dynamic_cast<AST_Annotation_Decl*>(
    root->lookup_by_name(fullname().c_str()));
}

AST_Expression::AST_ExprValue* get_annotation_member_ev(
  AST_Annotation_Appl* appl, const char* member_name)
{
  AST_Annotation_Member* member =
    dynamic_cast<AST_Annotation_Member*>((*appl)[member_name]);
  AST_Expression::AST_ExprValue* ev = 0;
  if (member) {
    AST_Expression* e = member->value();
    if (e) {
      ev = e->ev();
    }
  }
  if (!ev) {
    idl_global->err()->misc_error(
      (std::string("Found null pointer while getting value of member \"") +
        member_name + "\" of annotation \"" +
        appl->local_name()->get_string() + "\"").c_str(),
      appl);
    BE_abort();
  }
  return ev;
}

bool get_bool_annotation_member_value(
  AST_Annotation_Appl* appl, const char* member_name)
{
  return get_annotation_member_ev(appl, member_name)->u.bval;
}

ACE_UINT32 get_u32_annotation_member_value(
  AST_Annotation_Appl* appl, const char* member_name)
{
  return get_annotation_member_ev(appl, member_name)->u.ulval;
}

std::string get_str_annotation_member_value(
  AST_Annotation_Appl* appl, const char* member_name)
{
  UTL_String* idlstr = get_annotation_member_ev(appl, member_name)->u.strval;
  if (!idlstr) {
    idl_global->err()->misc_error(
      (std::string("Found null pointer while getting string value of member \"") +
        member_name + "\" of annotation \"" +
        appl->local_name()->get_string() + "\"").c_str(),
      appl);
    BE_abort();
  }
  return idlstr->get_string();
}

template<>
bool AnnotationWithValue<bool>::value_from_appl(
  AST_Annotation_Appl* appl) const
{
  return appl ? get_bool_annotation_member_value(appl, "value") : default_value();
}

template<>
ACE_UINT32 AnnotationWithValue<ACE_UINT32>::value_from_appl(
  AST_Annotation_Appl* appl) const
{
  return appl ? get_u32_annotation_member_value(appl, "value") : default_value();
}

template<>
std::string AnnotationWithValue<std::string>::value_from_appl(
  AST_Annotation_Appl* appl) const
{
  return appl ? get_str_annotation_member_value(appl, "value") : default_value();
}

// @key ======================================================================

std::string KeyAnnotation::definition() const
{
  return
    "@annotation key {\n"
    "  boolean value default TRUE;\n"
    "};\n";
}

std::string KeyAnnotation::name() const
{
  return "key";
}

bool KeyAnnotation::union_value(AST_Union* node) const
{
  return value_from_appl(node->disc_annotations().find(declaration()));
}

// @topic ====================================================================

TopicAnnotation::TopicAnnotation()
{
  platforms_.insert("*");
  platforms_.insert("DDS");
  platforms_.insert("OpenDDS");
}

std::string TopicAnnotation::definition() const
{
  return
    "@annotation topic {\n"
    "  string name default \"\";\n"
    "  string platform default \"*\";\n"
    "};\n";
}

std::string TopicAnnotation::name() const
{
  return "topic";
}

bool TopicAnnotation::node_value(AST_Decl* node) const
{
  if (node) {
    for (AST_Annotation_Appls::iterator i = node->annotations().begin();
        i != node->annotations().end(); ++i) {
      if (value_from_appl(i->get())) {
        return true;
      }
    }
  }
  return false;
}

bool TopicAnnotation::value_from_appl(AST_Annotation_Appl* appl) const
{
  if (appl && appl->annotation_decl() == declaration()) {
    return platforms_.count(get_str_annotation_member_value(appl, "platform"));
  }
  return false;
}

// @nested ===================================================================

std::string NestedAnnotation::definition() const
{
  return
    "@annotation nested {\n"
    "  boolean value default TRUE;\n"
    "};\n";
}

std::string NestedAnnotation::name() const
{
  return "nested";
}

// @default_nested ===========================================================

std::string DefaultNestedAnnotation::definition() const
{
  return
    "@annotation default_nested {\n"
    "  boolean value default TRUE;\n"
    "};\n";
}

std::string DefaultNestedAnnotation::name() const
{
  return "default_nested";
}

// @id =======================================================================

std::string IdAnnotation::definition() const
{
  return
    "@annotation id {\n"
    "  unsigned long value;\n"
    "};\n";
}

std::string IdAnnotation::name() const
{
  return "id";
}

// @autoid ===================================================================

std::string AutoidAnnotation::definition() const
{
  return
    "@annotation autoid {\n"
    "    enum AutoidKind {\n"
    "      SEQUENTIAL,\n"
    "      HASH\n"
    "    };\n"
    "    AutoidKind value default HASH;\n"
    "};\n";
}

std::string AutoidAnnotation::name() const
{
  return "autoid";
}

AutoidKind AutoidAnnotation::default_value() const
{
  return autoidkind_sequential;
}

// @hashid ===================================================================

std::string HashidAnnotation::definition() const
{
  return
    "@annotation hashid {\n"
    "  string value default \"\";"
    "};\n";
}

std::string HashidAnnotation::name() const
{
  return "hashid";
}

// @extensibility ============================================================

std::string ExtensibilityAnnotation::definition() const
{
  return
    "@annotation extensibility {\n"
    "  enum ExtensibilityKind {\n"
    "    FINAL,\n"
    "    APPENDABLE,\n"
    "    MUTABLE\n"
    "  };\n"
    "  ExtensibilityKind value;\n"
    "};\n";
}

std::string ExtensibilityAnnotation::name() const
{
  return "extensibility";
}

ExtensibilityKind ExtensibilityAnnotation::default_value() const
{
  return extensibilitykind_appendable;
}

// @final ====================================================================

std::string FinalAnnotation::definition() const
{
  return "@annotation final {};\n";
}

std::string FinalAnnotation::name() const
{
  return "final";
}

// @appendable ===============================================================

std::string AppendableAnnotation::definition() const
{
  return "@annotation appendable {};\n";
}

std::string AppendableAnnotation::name() const
{
  return "appendable";
}

// @mutable ==================================================================

std::string MutableAnnotation::definition() const
{
  return "@annotation mutable {};\n";
}

std::string MutableAnnotation::name() const
{
  return "mutable";
}

namespace OpenDDS {

  // @OpenDDS::data_representation ===========================================

  std::string DataRepresentationAnnotation::definition() const
  {
    return
      "@annotation data_representation {\n"
      "  enum Kind_t {\n"
      "    XCDR1,\n"
      "    XML,\n"
      "    XCDR2\n"
      "  };\n"
      "  Kind_t kind;\n"
      "};\n";
  }

  std::string DataRepresentationAnnotation::name() const
  {
    return "data_representation";
  }

  DataRepresentationKind DataRepresentationAnnotation::node_value(
    AST_Decl* node) const
  {
    DataRepresentationKind mask = data_representation_kind_none;
    if (node) {
      for (AST_Annotation_Appls::iterator i = node->annotations().begin();
          i != node->annotations().end(); ++i) {
        mask = value_from_appl(i->get());
      }
    }
    return mask ? mask : default_value();
  }

  DataRepresentationKind DataRepresentationAnnotation::default_value() const
  {
    return data_representation_kind_any;
  }

  DataRepresentationKind DataRepresentationAnnotation::value_from_appl(
    AST_Annotation_Appl* appl) const
  {
    if (appl && appl->annotation_decl() == declaration()) {
      switch (get_u32_annotation_member_value(appl, "kind")) {
      case 0:
        return data_representation_kind_xcdr1;
      case 1:
        return data_representation_kind_xml;
      case 2:
        return data_representation_kind_xcdr2;
      }
    }
    return data_representation_kind_none;
  }

}
