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

class PyArrayToFapiData
{
    public:
    PyArrayToFapiData(uint32_t i_id, PyObject * i_data);
    uint32_t getAndCheckDimensions(void);
    uint32_t copy(fapi2::AttributeData & io_data);

    private:
    uint32_t setAttributeArrayData(fapi2::AttributeData & io_data, uint32_t i_entry, PyObject * i_data);

    uint32_t iv_id;
    PyObject * iv_wList;
    uint32_t iv_dim;
    Py_ssize_t iv_wSize;
    Py_ssize_t iv_xSize;
    Py_ssize_t iv_ySize;
    Py_ssize_t iv_zSize;
};

PyArrayToFapiData::PyArrayToFapiData(uint32_t i_id, PyObject * i_data) :
    iv_id(i_id),
    iv_wList(i_data),
    iv_dim(0),
    iv_wSize(0),
    iv_xSize(0),
    iv_ySize(0),
    iv_zSize(0)
{ }

uint32_t PyArrayToFapiData::getAndCheckDimensions(void)
{
    iv_dim = 0;
    Py_ssize_t l_size = 1;
    if (PyList_Check(iv_wList))
    {
        iv_dim++;
        iv_wSize = PyList_Size(iv_wList);
        l_size *= iv_wSize;
        if (iv_wSize > 0)
        {
            PyObject * l_xList = PyList_GetItem(iv_wList, 0);
            if ((l_xList != NULL) && PyList_Check(l_xList))
            {
                iv_dim++;
                iv_xSize = PyList_Size(l_xList);
                l_size *= iv_xSize;
                if (iv_xSize > 0)
                {
                    PyObject * l_yList = PyList_GetItem(l_xList, 0);
                    if ((l_yList != NULL) && PyList_Check(l_yList))
                    {
                        iv_dim++;
                        iv_ySize = PyList_Size(l_yList);
                        l_size *= iv_ySize;
                        if (iv_ySize > 0)
                        {
                            PyObject * l_zList = PyList_GetItem(l_yList, 0);
                            if ((l_zList != NULL) && PyList_Check(l_zList))
                            {
                                iv_dim++;
                                iv_zSize = PyList_Size(l_zList);
                                l_size *= iv_zSize;
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        return ECMD_FAPI2_TYPE_MISMATCH_ERROR;
    }

    uint32_t l_attrType = 0;
    uint32_t l_numOfEntries = 0;
    uint32_t l_numOfBytes = 0;
    bool l_attrEnum = false;
    fapi2GetAttrInfo(static_cast<fapi2::AttributeId>(iv_id), l_attrType, l_numOfEntries, l_numOfBytes, l_attrEnum);

    if (l_size != l_numOfEntries)
    {
        return ECMD_FAPI2_ARRAY_SIZE_MISMATCH_ERROR;
    }
    return 0;
}

uint32_t PyArrayToFapiData::copy(fapi2::AttributeData & io_data)
{
    uint32_t rc = 0;
    if (iv_dim == 1)
    {
        for (uint32_t l_w = 0; l_w < iv_wSize; l_w++)
        {
            PyObject * l_w_data = PyList_GetItem(iv_wList, l_w);
            rc = setAttributeArrayData(io_data, l_w, l_w_data);
            if (rc)
            {
                return rc;
            }
        }
    }
    else if (iv_dim == 2)
    {
        for (uint32_t l_w = 0; l_w < iv_wSize; l_w++)
        {
            PyObject * l_xList = PyList_GetItem(iv_wList, l_w);
            Py_ssize_t l_xSize = PyList_Size(l_xList);
            if (iv_xSize != l_xSize)
            {
                return ECMD_FAPI2_ARRAY_SIZE_INCONSISTENT_ERROR;
            }
            for (uint32_t l_x = 0; l_x < iv_xSize; l_x++)
            {
                uint32_t l_entry = (l_w * iv_xSize) + l_x;
                PyObject * l_x_data = PyList_GetItem(l_xList, l_x);
                rc = setAttributeArrayData(io_data, l_entry, l_x_data);
                if (rc)
                {
                    return rc;
                }
            }
        }
    }
    else if (iv_dim == 3)
    {
        for (uint32_t l_w = 0; l_w < iv_wSize; l_w++)
        {
            PyObject * l_xList = PyList_GetItem(iv_wList, l_w);
            Py_ssize_t l_xSize = PyList_Size(l_xList);
            if (iv_xSize != l_xSize)
            {
                return ECMD_FAPI2_ARRAY_SIZE_INCONSISTENT_ERROR;
            }
            for (uint32_t l_x = 0; l_x < iv_xSize; l_x++)
            {
                PyObject * l_yList = PyList_GetItem(l_xList, l_x);
                Py_ssize_t l_ySize = PyList_Size(l_yList);
                if (iv_ySize != l_ySize)
                {
                    return ECMD_FAPI2_ARRAY_SIZE_INCONSISTENT_ERROR;
                }
                for (uint32_t l_y = 0; l_y < iv_ySize; l_y++)
                {
                    uint32_t l_entry = (l_w * iv_xSize * iv_ySize) +
                                       (l_x * iv_ySize) + l_y;
                    PyObject * l_y_data = PyList_GetItem(l_yList, l_y);
                    rc = setAttributeArrayData(io_data, l_entry, l_y_data);
                    if (rc)
                    {
                        return rc;
                    }
                }
            }
        }
    }
    else if (iv_dim == 4)
    {
        for (uint32_t l_w = 0; l_w < iv_wSize; l_w++)
        {
            PyObject * l_xList = PyList_GetItem(iv_wList, l_w);
            Py_ssize_t l_xSize = PyList_Size(l_xList);
            if (iv_xSize != l_xSize)
            {
                return ECMD_FAPI2_ARRAY_SIZE_INCONSISTENT_ERROR;
            }
            for (uint32_t l_x = 0; l_x < iv_xSize; l_x++)
            {
                PyObject * l_yList = PyList_GetItem(l_xList, l_x);
                Py_ssize_t l_ySize = PyList_Size(l_yList);
                if (iv_ySize != l_ySize)
                {
                    return ECMD_FAPI2_ARRAY_SIZE_INCONSISTENT_ERROR;
                }
                for (uint32_t l_y = 0; l_y < iv_ySize; l_y++)
                {
                    PyObject * l_zList = PyList_GetItem(l_yList, l_y);
                    Py_ssize_t l_zSize = PyList_Size(l_zList);
                    if (iv_zSize != l_zSize)
                    {
                        return ECMD_FAPI2_ARRAY_SIZE_INCONSISTENT_ERROR;
                    }
                    for (uint32_t l_z = 0; l_z < iv_zSize; l_z++)
                    {
                        uint32_t l_entry = (l_w * iv_xSize * iv_ySize * iv_zSize) +
                                           (l_x * iv_ySize * iv_zSize) +
                                           (l_y * iv_zSize) + l_z;
                        PyObject * l_z_data = PyList_GetItem(l_zList, l_z);
                        rc = setAttributeArrayData(io_data, l_entry, l_z_data);
                        if (rc)
                        {
                            return rc;
                        }
                    }
                }
            }
        }
    }
    return rc;
}

uint32_t PyArrayToFapiData::setAttributeArrayData(fapi2::AttributeData & io_data, uint32_t i_entry, PyObject * i_data)
{
    if (PyInt_Check(i_data))
    {
        long py_value = PyInt_AsLong(i_data);
        if (py_value == -1)
        {
            PyObject * err = PyErr_Occurred();
            if (err != NULL)
            {
                PyErr_Print();
                return ECMD_FAPI2_DATA_CONVERSION_ERROR;
            }
        }

        if (io_data.faValidMask == FAPI_ATTRIBUTE_TYPE_UINT8ARY)
            io_data.faUint8ary[i_entry] = py_value;
        else if (io_data.faValidMask == FAPI_ATTRIBUTE_TYPE_UINT32ARY)
            io_data.faUint32ary[i_entry] = py_value;
        else if (io_data.faValidMask == FAPI_ATTRIBUTE_TYPE_UINT64ARY)
            io_data.faUint64ary[i_entry] = py_value;
        else if (io_data.faValidMask == FAPI_ATTRIBUTE_TYPE_INT8ARY)
            io_data.faInt8ary[i_entry] = py_value;
        else if (io_data.faValidMask == FAPI_ATTRIBUTE_TYPE_INT32ARY)
            io_data.faInt32ary[i_entry] = py_value;
        else if (io_data.faValidMask == FAPI_ATTRIBUTE_TYPE_INT64ARY)
            io_data.faInt64ary[i_entry] = py_value;
        else if (io_data.faValidMask == FAPI_ATTRIBUTE_TYPE_UINT16ARY)
            io_data.faUint16ary[i_entry] = py_value;
        else if (io_data.faValidMask == FAPI_ATTRIBUTE_TYPE_INT16ARY)
            io_data.faInt16ary[i_entry] = py_value;
    }
    else
    {
        return ECMD_FAPI2_TYPE_MISMATCH_ERROR;
    }

    return 0;
}

uint32_t setPyAttribute(uint32_t i_id, PyObject * i_data, fapi2::AttributeData & io_data)
{
    if (io_data.faValidMask &
        (FAPI_ATTRIBUTE_TYPE_UINT8 |
         FAPI_ATTRIBUTE_TYPE_UINT32 |
         FAPI_ATTRIBUTE_TYPE_UINT64 |
         FAPI_ATTRIBUTE_TYPE_INT8 |
         FAPI_ATTRIBUTE_TYPE_INT32 |
         FAPI_ATTRIBUTE_TYPE_INT64 |
         FAPI_ATTRIBUTE_TYPE_UINT16 |
         FAPI_ATTRIBUTE_TYPE_INT16))
    {
        if (PyInt_Check(i_data))
        {
            long py_value = PyInt_AsLong(i_data);
            if (py_value == -1)
            {
                PyObject * err = PyErr_Occurred();
                if (err != NULL)
                {
                    PyErr_Print();
                    return ECMD_FAPI2_DATA_CONVERSION_ERROR;
                }
            }
            switch (io_data.faValidMask)
            {
                case FAPI_ATTRIBUTE_TYPE_UINT8:
                    io_data.faUint8 = py_value;
                    break;
                case FAPI_ATTRIBUTE_TYPE_UINT32:
                    io_data.faUint32 = py_value;
                    break;
                case FAPI_ATTRIBUTE_TYPE_UINT64:
                    io_data.faUint64 = py_value;
                    break;
                case FAPI_ATTRIBUTE_TYPE_INT8:
                    io_data.faInt8 = py_value;
                    break;
                case FAPI_ATTRIBUTE_TYPE_INT32:
                    io_data.faInt32 = py_value;
                    break;
                case FAPI_ATTRIBUTE_TYPE_INT64:
                    io_data.faInt64 = py_value;
                    break;
                case FAPI_ATTRIBUTE_TYPE_UINT16:
                    io_data.faUint16 = py_value;
                    break;
                case FAPI_ATTRIBUTE_TYPE_INT16:
                    io_data.faInt16 = py_value;
                    break;
                default:
                    break;
            }
        }
        else
        {
            return ECMD_FAPI2_TYPE_MISMATCH_ERROR;
        }
    }
    else if (io_data.faValidMask &
             (FAPI_ATTRIBUTE_TYPE_UINT8ARY |
              FAPI_ATTRIBUTE_TYPE_UINT32ARY |
              FAPI_ATTRIBUTE_TYPE_UINT64ARY |
              FAPI_ATTRIBUTE_TYPE_INT8ARY |
              FAPI_ATTRIBUTE_TYPE_INT32ARY |
              FAPI_ATTRIBUTE_TYPE_INT64ARY |
              FAPI_ATTRIBUTE_TYPE_UINT16ARY |
              FAPI_ATTRIBUTE_TYPE_INT16ARY))
    {
        PyArrayToFapiData p(i_id, i_data);
        uint32_t rc = p.getAndCheckDimensions();
        if (rc)
            return rc;

        rc = p.copy(io_data);
        if (rc)
            return rc;
    }
    else
    {
        return ECMD_FAPI2_UNKNOWN_TYPE_ERROR;
    }
    return 0;
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

def fapi2SetAttr(i_target, i_id, i_data):
    (rc, attr_id) = fapi2AttributeStringToId(i_id)
    if (rc):
        return rc
    attr_data = createAttribute(attr_id)
    rc = setPyAttribute(attr_id, i_data, attr_data)
    if (rc):
        destroyAttribute(attr_data)
        return rc
    rc = _ecmd.fapi2SetAttribute(i_target, attr_id, attr_data)
    destroyAttribute(attr_data)
    return rc
%}
