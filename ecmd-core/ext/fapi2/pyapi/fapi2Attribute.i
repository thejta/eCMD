%inline %{

fapi2::AttributeData createAttribute(uint32_t i_id)
{
    uint32_t l_attrType = 0;
    uint32_t l_numOfEntries = 0;
    uint32_t l_numOfBytes = 0;
    bool l_attrEnum = false;
    fapi2GetAttrInfo(static_cast<fapi2::AttributeId>(i_id), l_attrType, l_numOfEntries, l_numOfBytes, l_attrEnum);

    fapi2::AttributeData l_data;
    l_data.faValidMask = l_attrType;
    switch (l_attrType)
    {
        // FIXME allocate data
        case FAPI_ATTRIBUTE_TYPE_UINT8ARY:
        case FAPI_ATTRIBUTE_TYPE_UINT32ARY:
        case FAPI_ATTRIBUTE_TYPE_UINT64ARY:
        case FAPI_ATTRIBUTE_TYPE_INT8ARY:
        case FAPI_ATTRIBUTE_TYPE_INT32ARY:
        case FAPI_ATTRIBUTE_TYPE_INT64ARY:
        case FAPI_ATTRIBUTE_TYPE_UINT16ARY:
        case FAPI_ATTRIBUTE_TYPE_INT16ARY:
        default:
            break;
    }

    return l_data;
}

void destroyAttribute(fapi2::AttributeData & i_data)
{
    switch (i_data.faValidMask)
    {
        // FIXME deallocate data
        case FAPI_ATTRIBUTE_TYPE_UINT8ARY:
        case FAPI_ATTRIBUTE_TYPE_UINT32ARY:
        case FAPI_ATTRIBUTE_TYPE_UINT64ARY:
        case FAPI_ATTRIBUTE_TYPE_INT8ARY:
        case FAPI_ATTRIBUTE_TYPE_INT32ARY:
        case FAPI_ATTRIBUTE_TYPE_INT64ARY:
        case FAPI_ATTRIBUTE_TYPE_UINT16ARY:
        case FAPI_ATTRIBUTE_TYPE_INT16ARY:
        default:
            break;
    }
}

PyObject * getPyAttribute(fapi2::AttributeData & i_data)
{
    PyObject * return_val = NULL;
    switch (i_data.faValidMask)
    {
        // FIXME handle ARRAY
        case FAPI_ATTRIBUTE_TYPE_UINT8:
            return_val = PyInt_FromLong(i_data.faUint8);
            break;
        case FAPI_ATTRIBUTE_TYPE_UINT32:
            return_val = PyInt_FromLong(i_data.faUint32);
            break;
        case FAPI_ATTRIBUTE_TYPE_UINT64:
            return_val = PyInt_FromLong(i_data.faUint64);
            break;
        case FAPI_ATTRIBUTE_TYPE_INT8:
            return_val = PyInt_FromLong(i_data.faInt8);
            break;
        case FAPI_ATTRIBUTE_TYPE_INT32:
            return_val = PyInt_FromLong(i_data.faInt32);
            break;
        case FAPI_ATTRIBUTE_TYPE_INT64:
            return_val = PyInt_FromLong(i_data.faInt64);
            break;
        case FAPI_ATTRIBUTE_TYPE_UINT16:
            return_val = PyInt_FromLong(i_data.faUint16);
            break;
        case FAPI_ATTRIBUTE_TYPE_INT16:
            return_val = PyInt_FromLong(i_data.faInt16);
            break;
        default:
            Py_RETURN_NONE;
            break;
    }
    return return_val;
}

%}

%apply int &OUTPUT {fapi2::AttributeId & o_attrId};

%pythoncode %{
def fapi2GetAttr(i_target, i_id):
    (rc, attr_id) = fapi2AttributeStringToId(i_id)
    attr_data = createAttribute(attr_id)
    rc = _ecmd.fapi2GetAttribute(i_target, attr_id, attr_data)
    val = getPyAttribute(attr_data)
    destroyAttribute(attr_data)
    return val
%}
