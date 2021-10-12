const fs = require("fs");
const getNameFromSchema = require("./getNameFromSchema");
const indent = require("./indent");
const lodash = require("lodash");
const path = require("path");
const resolveProperty = require("./resolveProperty");
const unindent = require("./unindent");

function generate(options, schema) {
  const {
    schemaCache,
    outputDir,
    readerOutputDir,
    config,
    namespace,
  } = options;

  const name = getNameFromSchema(config, schema);
  const thisConfig = config.classes[schema.title] || {};

  console.log(`Generating ${name}`);

  schemaCache.pushContext(schema);

  let base = "CesiumUtility::ExtensibleObject";
  if (schema.allOf && schema.allOf.length > 0 && schema.allOf[0].$ref) {
    const baseSchema = schemaCache.load(schema.allOf[0].$ref);
    base = getNameFromSchema(config, baseSchema);
  }

  const required = schema.required || [];

  const properties = Object.keys(schema.properties)
    .map((key) =>
      resolveProperty(
        schemaCache,
        config,
        name,
        key,
        schema.properties[key],
        required,
        namespace
      )
    )
    .filter((property) => property !== undefined);

  const localTypes = lodash.uniq(
    lodash.flatten(properties.map((property) => property.localTypes))
  );

  schemaCache.popContext();

  let headers = lodash.uniq([
    `"Library.h"`,
    getIncludeFromName(base),
    ...lodash.flatten(properties.map((property) => property.headers)),
  ]);

  // Prevent header from including itself for recursive types like Tile
  headers = headers.filter((header) => {
    return header !== `"${name}.h"`;
  });

  headers.sort();

  // prettier-ignore
  const header = `
        // This file was generated by generate-gltf-classes.
        // DO NOT EDIT THIS FILE!
        #pragma once

        ${headers.map((header) => `#include ${header}`).join("\n")}

        namespace ${namespace} {
            /**
             * @brief ${schema.description ? schema.description : schema.title}
             */
            struct ${namespace.toUpperCase()}_API ${name}${thisConfig.toBeInherited ? "Spec" : (thisConfig.isBaseClass ? "" : " final")} : public ${base} {
                static inline constexpr const char* TypeName = "${name}";
                ${thisConfig.extensionName ? `static inline constexpr const char* ExtensionName = "${thisConfig.extensionName}";` : ""}

                ${indent(localTypes.join("\n\n"), 16)}

                ${indent(
                  properties
                    .map((property) => formatProperty(property))
                    .filter(propertyText => propertyText !== undefined)
                    .join("\n\n"),
                  16
                )}
                ${thisConfig.toBeInherited ? privateSpecConstructor(name) : ""}
            };
        }
    `;

  const headerOutputDir = path.join(outputDir, "include", namespace);
  fs.mkdirSync(headerOutputDir, { recursive: true });
  const headerOutputPath = path.join(
    headerOutputDir,
    `${name}${thisConfig.toBeInherited ? "Spec" : ""}.h`
  );
  fs.writeFileSync(headerOutputPath, unindent(header), "utf-8");

  let readerHeaders = lodash.uniq([
    getReaderIncludeFromName(base),
    `"${namespace}/${name}.h"`,
    ...lodash.flatten(properties.map((property) => property.readerHeaders)),
  ]);

  // Prevent header from including itself for recursive types like Tile
  readerHeaders = readerHeaders.filter((readerHeader) => {
    return readerHeader !== `"${name}JsonHandler.h"`;
  });

  readerHeaders.sort();

  const readerLocalTypes = lodash.uniq(
    lodash.flatten(properties.map((property) => property.readerLocalTypes))
  );

  const baseReader = getReaderName(base);

  // prettier-ignore
  const readerHeader = `
        // This file was generated by generate-gltf-classes.
        // DO NOT EDIT THIS FILE!
        #pragma once

        ${readerHeaders.map((header) => `#include ${header}`).join("\n")}

        namespace CesiumJsonReader {
          class ExtensionReaderContext;
        }

        namespace ${namespace} {
          class ${name}JsonHandler : public ${baseReader}${thisConfig.extensionName ? `, public CesiumJsonReader::IExtensionJsonHandler` : ""} {
          public:
            using ValueType = ${name};

            ${thisConfig.extensionName ? `static inline constexpr const char* ExtensionName = "${thisConfig.extensionName}";` : ""}

            ${name}JsonHandler(const CesiumJsonReader::ExtensionReaderContext& context) noexcept;
            void reset(IJsonHandler* pParentHandler, ${name}* pObject);

            virtual IJsonHandler* readObjectKey(const std::string_view& str) override;

            ${thisConfig.extensionName ? `
            virtual void reset(IJsonHandler* pParentHandler, CesiumUtility::ExtensibleObject& o, const std::string_view& extensionName) override;

            virtual IJsonHandler* readNull() override {
              return ${baseReader}::readNull();
            };
            virtual IJsonHandler* readBool(bool b) override {
              return ${baseReader}::readBool(b);
            }
            virtual IJsonHandler* readInt32(int32_t i) override {
              return ${baseReader}::readInt32(i);
            }
            virtual IJsonHandler* readUint32(uint32_t i) override {
              return ${baseReader}::readUint32(i);
            }
            virtual IJsonHandler* readInt64(int64_t i) override {
              return ${baseReader}::readInt64(i);
            }
            virtual IJsonHandler* readUint64(uint64_t i) override {
              return ${baseReader}::readUint64(i);
            }
            virtual IJsonHandler* readDouble(double d) override {
              return ${baseReader}::readDouble(d);
            }
            virtual IJsonHandler* readString(const std::string_view& str) override {
              return ${baseReader}::readString(str);
            }
            virtual IJsonHandler* readObjectStart() override {
              return ${baseReader}::readObjectStart();
            }
            virtual IJsonHandler* readObjectEnd() override {
              return ${baseReader}::readObjectEnd();
            }
            virtual IJsonHandler* readArrayStart() override {
              return ${baseReader}::readArrayStart();
            }
            virtual IJsonHandler* readArrayEnd() override {
              return ${baseReader}::readArrayEnd();
            }
            virtual void reportWarning(const std::string& warning, std::vector<std::string>&& context = std::vector<std::string>()) override {
              ${baseReader}::reportWarning(warning, std::move(context));
            }
            ` : ""}

          protected:
            IJsonHandler* readObjectKey${name}(const std::string& objectType, const std::string_view& str, ${name}& o);

          private:
            ${indent(readerLocalTypes.join("\n\n"), 12)}

            ${name}* _pObject = nullptr;
            ${indent(
              properties
                .map((property) => formatReaderProperty(property))
                .join("\n"),
              12
            )}
          };
        }
  `;

  const readerHeaderOutputDir = path.join(readerOutputDir, "generated");
  fs.mkdirSync(readerHeaderOutputDir, { recursive: true });
  const readerHeaderOutputPath = path.join(
    readerHeaderOutputDir,
    name + "JsonHandler.h"
  );
  fs.writeFileSync(readerHeaderOutputPath, unindent(readerHeader), "utf-8");

  const readerLocalTypesImpl = lodash.uniq(
    lodash.flatten(properties.map((property) => property.readerLocalTypesImpl))
  );

  const readerHeadersImpl = lodash.uniq([
    ...lodash.flatten(properties.map((property) => property.readerHeadersImpl)),
  ]);
  readerHeadersImpl.sort();

  function generateReaderOptionsInitializerList(properties, varName) {
    const initializerList = properties
      .filter((p) => p.readerType.toLowerCase().indexOf("jsonhandler") != -1)
      .map(
        (p) => `_${p.name}(${p.schemas && p.schemas.length > 0 ? varName : ""})`
      )
      .join(", ");
    return initializerList == "" ? "" : ", " + initializerList;
  }
  // prettier-ignore
  const readerImpl = `
        // This file was generated by generate-gltf-classes.
        // DO NOT EDIT THIS FILE!
        #include "${name}JsonHandler.h"
        #include "${namespace}/${name}.h"
        ${readerHeadersImpl.map((header) => `#include ${header}`).join("\n")}
        #include <cassert>
        #include <string>

        using namespace ${namespace};

        ${name}JsonHandler::${name}JsonHandler(const CesiumJsonReader::ExtensionReaderContext& context) noexcept : ${baseReader}(context)${generateReaderOptionsInitializerList(properties, 'context')} {}

        void ${name}JsonHandler::reset(CesiumJsonReader::IJsonHandler* pParentHandler, ${name}* pObject) {
          ${baseReader}::reset(pParentHandler, pObject);
          this->_pObject = pObject;
        }

        CesiumJsonReader::IJsonHandler* ${name}JsonHandler::readObjectKey(const std::string_view& str) {
          assert(this->_pObject);
          return this->readObjectKey${name}(${name}::TypeName, str, *this->_pObject);
        }

        ${thisConfig.extensionName ? `
        void ${name}JsonHandler::reset(CesiumJsonReader::IJsonHandler* pParentHandler, CesiumUtility::ExtensibleObject& o, const std::string_view& extensionName) {
          std::any& value =
              o.extensions.emplace(extensionName, ${name}())
                  .first->second;
          this->reset(
              pParentHandler,
              &std::any_cast<${name}&>(value));
        }
        ` : ""}

        CesiumJsonReader::IJsonHandler* ${name}JsonHandler::readObjectKey${name}(const std::string& objectType, const std::string_view& str, ${name}& o) {
          using namespace std::string_literals;

          ${indent(
            properties
              .map((property) => formatReaderPropertyImpl(property))
              .join("\n"),
            10
          )}

          return this->readObjectKey${removeNamespace(base)}(objectType, str, *this->_pObject);
        }

        ${indent(readerLocalTypesImpl.join("\n\n"), 8)}
  `;

  if (options.oneHandlerFile) {
    const readerSourceOutputPath = path.join(
      readerHeaderOutputDir,
      "GeneratedJsonHandlers.cpp"
    );
    fs.appendFileSync(readerSourceOutputPath, unindent(readerImpl), "utf-8");
  } else {
    const readerSourceOutputPath = path.join(
      readerHeaderOutputDir,
      name + "JsonHandler.cpp"
    );
    fs.writeFileSync(readerSourceOutputPath, unindent(readerImpl), "utf-8");
  }

  return lodash.uniq(
    lodash.flatten(properties.map((property) => property.schemas))
  );
}

function formatProperty(property) {
  if (!property.type) {
    return undefined;
  }

  let result = "";

  result += `/**\n * @brief ${property.briefDoc || property.name}\n`;
  if (property.fullDoc) {
    result += ` *\n * ${property.fullDoc.split("\n").join("\n * ")}\n`;
  }

  result += ` */\n`;

  result += `${property.type} ${property.name}`;

  if (property.defaultValue !== undefined) {
    result += " = " + property.defaultValue;
  } else if (property.needsInitialization) {
    result += " = " + property.type + "()";
  }

  result += ";";

  return result;
}

function formatReaderProperty(property) {
  return `${property.readerType} _${property.name};`;
}

function formatReaderPropertyImpl(property) {
  return `if ("${property.name}"s == str) return property("${property.name}", this->_${property.name}, o.${property.name});`;
}

function privateSpecConstructor(name) {
  return `
    private:
      /**
       * @brief This class is not meant to be instantiated directly. Use {@link ${name}} instead.
       */
      ${name}Spec() = default;
      friend struct ${name};
  `;
}

const qualifiedTypeNameRegex = /(?:(?<namespace>.+)::)?(?<name>.+)/;

function getIncludeFromName(name) {
  const pieces = name.match(qualifiedTypeNameRegex);
  if (pieces && pieces.groups && pieces.groups.namespace) {
    return `<${pieces.groups.namespace}/${pieces.groups.name}.h>`;
  } else {
    return `"${name}.h"`;
  }
}

function getReaderIncludeFromName(name) {
  const pieces = name.match(qualifiedTypeNameRegex);
  if (pieces && pieces.groups && pieces.groups.namespace) {
    let namespace = pieces.groups.namespace;

    // CesiumUtility types have their corresponding reader classes in CesiumJsonReader
    if (namespace === "CesiumUtility") {
      namespace = "CesiumJsonReader";
    }
    return `<${namespace}/${pieces.groups.name}JsonHandler.h>`;
  } else {
    return `"${name}JsonHandler.h"`;
  }
}

function getReaderName(name) {
  const pieces = name.match(qualifiedTypeNameRegex);
  if (pieces && pieces.groups && pieces.groups.namespace) {
    let namespace = pieces.groups.namespace;

    // CesiumUtility types have their corresponding reader classes in CesiumJsonReader
    if (namespace === "CesiumUtility") {
      namespace = "CesiumJsonReader";
    }
    return `${namespace}::${pieces.groups.name}JsonHandler`;
  } else {
    return `${name}JsonHandler`;
  }
}

function removeNamespace(name) {
  const pieces = name.match(qualifiedTypeNameRegex);
  if (pieces && pieces.groups && pieces.groups.namespace) {
    return pieces.groups.name;
  } else {
    return name;
  }
}

module.exports = generate;
