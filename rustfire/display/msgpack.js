/// Parses MessagePack data.
export function parse(data) {
    let bytes = new Uint8Array(data);
    let dataView = new DataView(data);

    let index = 0;
    let decoder = new TextDecoder("UTF-8");

    /// Parses a map with the specified amount of entries.
    function nextMap(size) {
        let dict = {};

        for (let i = 0; i < size; i++) {
            let k = nextValue();
            if (typeof k != "string")
                throw new Error("map key is not a string");
            dict[k] = nextValue();
        }

        return dict;
    }

    /// Parses an array with the specified amount of entries.
    function nextArray(size) {
        let values = [];
        for (let i = 0; i < size; i++) values.push(nextValue());
        return values;
    }

    /// Parses a string with the specified length.
    function nextString(size) {
        let encoded = data.slice(index, index + size);
        index += size;

        return decoder.decode(encoded);
    }

    /// Parses a signed integer with the specified length.
    function nextInt(size) {
        let value = (bytes[index++] << 24) >> 24;
        for (let i = 1; i < size; i++) value = value * 256 + bytes[index++];
        return value;
    }

    /// Parses an unsigned integer with the specified length.
    function nextUInt(size) {
        let value = 0;
        for (let i = 0; i < size; i++) value = value * 256 + bytes[index++];
        return value;
    }

    /// Parses the specified amount of bytes.
    function nextBin(size) {
        let values = [];
        for (let i = 0; i < size; i++) values[i] = bytes[index++];
        return values;
    }

    /// Parses a 32 bit float.
    function nextF32() {
        index += 4;
        return dataView.getFloat32(index - 4);
    }

    /// Parses a 64 bit float.
    function nextF64() {
        index += 8;
        return dataView.getFloat64(index - 8);
    }

    /// Parses any value.
    function nextValue() {
        let byte = bytes[index++];

        if (byte == 0xc1) throw new Error("invalid data");
        if ((byte >= 0xc7 && byte <= 0xc9) || (byte >= 0xd4 && byte <= 0xd8))
            throw new Error("extension types are unsupported");

        if (byte <= 0x7f) return byte;
        if (byte <= 0x8f) return nextMap(byte - 0x80);
        if (byte <= 0x9f) return nextArray(byte - 0x90);
        if (byte <= 0xbf) return nextString(byte - 0xa0);
        if (byte == 0xc0) return null;
        if (byte == 0xc2) return false;
        if (byte == 0xc3) return true;
        if (byte <= 0xc6) return nextBin(nextUInt(1 << (byte - 0xc4)));
        if (byte == 0xca) return nextF32();
        if (byte == 0xcb) return nextF64();
        if (byte <= 0xcf) return nextUInt(1 << (byte - 0xcc));
        if (byte <= 0xd3) return nextInt(1 << (byte - 0xd0));
        if (byte <= 0xdb) return nextString(1 << (byte - 0xd9));
        if (byte <= 0xdd) return nextArray(nextUInt(2 << (byte - 0xdc)));
        if (byte <= 0xdf) return nextMap(nextUInt(2 << (byte - 0xde)));
        return (byte << 24) >> 24;
    }

    let value = nextValue();
    if (index != data.byteLength)
        throw new Error("extra bytes in encoded data");

    return value;
}
