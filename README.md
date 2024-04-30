# pythider
pythider is a tool to avoid getting blacklisted from the pytguard protection service &or to prevent being identified as blacklisted.
This allows for continuous reversal and normal behaviour of applications even after banned.

# The Flaw:
By hooking memcpy and memcmp, we can easily inspect the whole communication of pytguard, modifying both the responses and the inputs.

# How to use
1) Inject pythider.dll into a target process protected with PYTGuard.
2) profit $$$
