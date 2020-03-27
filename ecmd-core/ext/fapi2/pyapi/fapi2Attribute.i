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
    l_data.faMode = FAPI_ATTRIBUTE_MODE_DIMS;
    switch (l_attrType)
    {
        case FAPI_ATTRIBUTE_TYPE_UINT8ARY:
            l_data.faUint8ary = new uint8_t[l_numOfEntries];
            break;
        case FAPI_ATTRIBUTE_TYPE_UINT32ARY:
            l_data.faUint32ary = new uint32_t[l_numOfEntries];
            break;
        case FAPI_ATTRIBUTE_TYPE_UINT64ARY:
            l_data.faUint64ary = new uint64_t[l_numOfEntries];
            break;
        case FAPI_ATTRIBUTE_TYPE_INT8ARY:
            l_data.faInt8ary = new int8_t[l_numOfEntries];
            break;
        case FAPI_ATTRIBUTE_TYPE_INT32ARY:
            l_data.faInt32ary = new int32_t[l_numOfEntries];
            break;
        case FAPI_ATTRIBUTE_TYPE_INT64ARY:
            l_data.faInt64ary = new int64_t[l_numOfEntries];
            break;
        case FAPI_ATTRIBUTE_TYPE_UINT16ARY:
            l_data.faUint16ary = new uint16_t[l_numOfEntries];
            break;
        case FAPI_ATTRIBUTE_TYPE_INT16ARY:
            l_data.faInt16ary = new int16_t[l_numOfEntries];
            break;
        default:
            break;
    }

    return l_data;
}

void destroyAttribute(fapi2::AttributeData & i_data)
{
    switch (i_data.faValidMask)
    {
        case FAPI_ATTRIBUTE_TYPE_UINT8ARY:
            delete [] i_data.faUint8ary;
            break;
        case FAPI_ATTRIBUTE_TYPE_UINT32ARY:
            delete [] i_data.faUint32ary;
            break;
        case FAPI_ATTRIBUTE_TYPE_UINT64ARY:
            delete [] i_data.faUint64ary;
            break;
        case FAPI_ATTRIBUTE_TYPE_INT8ARY:
            delete [] i_data.faInt8ary;
            break;
        case FAPI_ATTRIBUTE_TYPE_INT32ARY:
            delete [] i_data.faInt32ary;
            break;
        case FAPI_ATTRIBUTE_TYPE_INT64ARY:
            delete [] i_data.faInt64ary;
            break;
        case FAPI_ATTRIBUTE_TYPE_UINT16ARY:
            delete [] i_data.faUint16ary;
            break;
        case FAPI_ATTRIBUTE_TYPE_INT16ARY:
            delete [] i_data.faInt16ary;
            break;
        default:
            break;
    }
}

PyObject * getPyDataFromArray(const fapi2::AttributeData & i_data, const uint32_t i_entry)
{
    PyObject * l_entryObject = NULL;
    if (i_data.faValidMask == FAPI_ATTRIBUTE_TYPE_UINT8ARY)
        l_entryObject = PyInt_FromLong(i_data.faUint8ary[i_entry]);
    else if (i_data.faValidMask == FAPI_ATTRIBUTE_TYPE_UINT32ARY)
        l_entryObject = PyInt_FromLong(i_data.faUint32ary[i_entry]);
    else if (i_data.faValidMask == FAPI_ATTRIBUTE_TYPE_UINT64ARY)
        l_entryObject = PyInt_FromLong(i_data.faUint64ary[i_entry]);
    else if (i_data.faValidMask == FAPI_ATTRIBUTE_TYPE_INT8ARY)
        l_entryObject = PyInt_FromLong(i_data.faInt8ary[i_entry]);
    else if (i_data.faValidMask == FAPI_ATTRIBUTE_TYPE_INT32ARY)
        l_entryObject = PyInt_FromLong(i_data.faInt32ary[i_entry]);
    else if (i_data.faValidMask == FAPI_ATTRIBUTE_TYPE_INT64ARY)
        l_entryObject = PyInt_FromLong(i_data.faInt64ary[i_entry]);
    else if (i_data.faValidMask == FAPI_ATTRIBUTE_TYPE_UINT16ARY)
        l_entryObject = PyInt_FromLong(i_data.faUint16ary[i_entry]);
    else if (i_data.faValidMask == FAPI_ATTRIBUTE_TYPE_INT16ARY)
        l_entryObject = PyInt_FromLong(i_data.faInt16ary[i_entry]);
    return l_entryObject;
}

PyObject * getPyAttribute(uint32_t i_id, fapi2::AttributeData & i_data)
{
    PyObject * return_val = NULL;
    switch (i_data.faValidMask)
    {
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
        case FAPI_ATTRIBUTE_TYPE_UINT8ARY:
        case FAPI_ATTRIBUTE_TYPE_UINT32ARY:
        case FAPI_ATTRIBUTE_TYPE_UINT64ARY:
        case FAPI_ATTRIBUTE_TYPE_INT8ARY:
        case FAPI_ATTRIBUTE_TYPE_INT32ARY:
        case FAPI_ATTRIBUTE_TYPE_INT64ARY:
        case FAPI_ATTRIBUTE_TYPE_UINT16ARY:
        case FAPI_ATTRIBUTE_TYPE_INT16ARY:
            {
                uint32_t l_attrType = 0;
                uint32_t l_numOfEntries = 0;
                uint32_t l_numOfBytes = 0;
                bool l_attrEnum = false;
                fapi2GetAttrInfo(static_cast<fapi2::AttributeId>(i_id), l_attrType, l_numOfEntries, l_numOfBytes, l_attrEnum);

                if ((i_data.faNumOfArrayDim == 0) || (i_data.faNumOfArrayDim == 1))
                {
                    return_val = PyList_New(l_numOfEntries);
                    for (uint32_t l_entry = 0; l_entry < l_numOfEntries; l_entry++)
                    {
                        PyList_SetItem(return_val, l_entry, getPyDataFromArray(i_data, l_entry));
                    }
                }
                else if (i_data.faNumOfArrayDim == 2)
                {
                    return_val = PyList_New(i_data.faSizeOfArrayDimW);
                    for (uint32_t l_w = 0; l_w < i_data.faSizeOfArrayDimW; l_w++)
                    {
                        PyObject * l_xList = PyList_New(i_data.faSizeOfArrayDimX);
                        PyList_SetItem(return_val, l_w, l_xList);
                        for (uint32_t l_x = 0; l_x < i_data.faSizeOfArrayDimX; l_x++)
                        {
                            uint32_t l_entry = (l_w * i_data.faSizeOfArrayDimX) + l_x;
                            PyList_SetItem(l_xList, l_x, getPyDataFromArray(i_data, l_entry));
                        }
                    }
                }
                else if (i_data.faNumOfArrayDim == 3)
                {
                    return_val = PyList_New(i_data.faSizeOfArrayDimW);
                    for (uint32_t l_w = 0; l_w < i_data.faSizeOfArrayDimW; l_w++)
                    {
                        PyObject * l_xList = PyList_New(i_data.faSizeOfArrayDimX);
                        PyList_SetItem(return_val, l_w, l_xList);
                        for (uint32_t l_x = 0; l_x < i_data.faSizeOfArrayDimX; l_x++)
                        {
                            PyObject * l_yList = PyList_New(i_data.faSizeOfArrayDimY);
                            PyList_SetItem(l_xList, l_x, l_yList);
                            for (uint32_t l_y = 0; l_y < i_data.faSizeOfArrayDimY; l_y++)
                            {
                                uint32_t l_entry = (l_w * i_data.faSizeOfArrayDimX * i_data.faSizeOfArrayDimY) +
                                                   (l_x * i_data.faSizeOfArrayDimY) + l_y;
                                PyList_SetItem(l_yList, l_y, getPyDataFromArray(i_data, l_entry));
                            }
                        }
                    }
                }
                else if (i_data.faNumOfArrayDim == 4)
                {
                    return_val = PyList_New(i_data.faSizeOfArrayDimW);
                    for (uint32_t l_w = 0; l_w < i_data.faSizeOfArrayDimW; l_w++)
                    {
                        PyObject * l_xList = PyList_New(i_data.faSizeOfArrayDimX);
                        PyList_SetItem(return_val, l_w, l_xList);
                        for (uint32_t l_x = 0; l_x < i_data.faSizeOfArrayDimX; l_x++)
                        {
                            PyObject * l_yList = PyList_New(i_data.faSizeOfArrayDimY);
                            PyList_SetItem(l_xList, l_x, l_yList);
                            for (uint32_t l_y = 0; l_y < i_data.faSizeOfArrayDimY; l_y++)
                            {
                                PyObject * l_zList = PyList_New(i_data.faSizeOfArrayDimZ);
                                PyList_SetItem(l_yList, l_y, l_zList);
                                for (uint32_t l_z = 0; l_z < i_data.faSizeOfArrayDimZ; l_z++)
                                {
                                    uint32_t l_entry = (l_w * i_data.faSizeOfArrayDimX * i_data.faSizeOfArrayDimY * i_data.faSizeOfArrayDimZ) +
                                                       (l_x * i_data.faSizeOfArrayDimY * i_data.faSizeOfArrayDimZ) +
                                                       (l_y * i_data.faSizeOfArrayDimZ) + l_z;
                                    PyList_SetItem(l_zList, l_z, getPyDataFromArray(i_data, l_entry));
                                }
                            }
                        }
                    }
                }
                else
                    Py_RETURN_NONE;
            }
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
    if (rc):
        return (rc, None)
    attr_data = createAttribute(attr_id)
    rc = _ecmd.fapi2GetAttribute(i_target, attr_id, attr_data)
    if (rc):
        destroyAttribute(attr_data)
        return (rc, None)
    val = getPyAttribute(attr_id, attr_data)
    destroyAttribute(attr_data)
    return (int(0), val)
%}
