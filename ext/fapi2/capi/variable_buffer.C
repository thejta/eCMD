#include <variable_buffer.H>

namespace fapi2
{
    variable_buffer::
    variable_buffer(bits_type i_value):
        buffer_base(i_value),
        iv_perceived_bit_length(i_value)
    {
        static_assert(std::is_same<unit_type, uint32_t>::value,
                      "code currently needs unit_type to be a unit32_t");
    }

    variable_buffer::
    variable_buffer(const std::initializer_list<unit_type>& i_value):
        buffer_base(i_value),
        iv_perceived_bit_length(i_value.size() * sizeof(unit_type) * 8)
    {
        static_assert(std::is_same<unit_type, uint32_t>::value,
                      "code currently needs unit_type to be a unit32_t");
    }

    //
    // Insert another variable_bufer
    //
    template<>
        fapi2::ReturnCode variable_buffer::insert(const variable_buffer& i_data,
                                                  bits_type i_targetStart,
                                                  bits_type i_len,
                                                  bits_type i_sourceStart)
    {
        return _insert((unit_type*)&(i_data()[0]), i_data.getBitLength(),
                       &(iv_data[0]), getBitLength(),
                       i_sourceStart, i_targetStart, i_len);
    }

    //
    //
    // variable_buffer insert from right
    //
    template<>
        fapi2::ReturnCode variable_buffer::insertFromRight(const variable_buffer& i_data,
                                                           bits_type i_targetStart,
                                                           bits_type i_len)
    {
        const bits_type bit_length_of_source = i_data.getBitLength();
        _insertFromRight(i_data, bit_length_of_source, i_targetStart, i_len);
    }

    //
    // Extract in to another variable_bufer
    //
    template<>
        fapi2::ReturnCode variable_buffer::extract(variable_buffer& i_data,
                                                   bits_type i_start,
                                                   bits_type i_len) const
    {
        // If thy didn't pass an i_len, assume they want all the data
        // which will fit.
        if (i_len == ~0)
        {
            i_len = i_data.getBitLength();
        }
        return _insert((container_unit*)&iv_data[0], getBitLength(),
                       &(i_data()[0]), i_data.getBitLength(),
                       i_start, 0U, i_len);
    }

}
