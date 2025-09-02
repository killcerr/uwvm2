## Rationale for Deprecating uwvm1 (formerly “uwvm”) and Rearchitecting as uwvm2

### Status
- **uwvm1**: Formerly [uwvm](https://github.com/UlteSoft/uwvm), deprecated. No new features; only archival for historical reference.
- **uwvm2**: Actively maintained and recommended for all new development and migrations.

### Summary
uwvm1 has been retired due to architectural limitations in its parser design, gaps in WebAssembly (Wasm) specification compliance, and insufficiently verified memory safety. uwvm2 is a ground-up rearchitecture that adopts Concept-Oriented Programming (COP) to make the system extensible, specification-driven, and measurably safer, with explicit memory-safety annotations that cover even SIMD paths.

### Why uwvm1 is Deprecated
1) **Non‑extensible, POP‑style parser architecture**
   - uwvm1’s parser followed a procedural/POP-style, stack‑pop–centric architecture with tight coupling and cross‑cutting concerns.
   - Extending or evolving the parser commonly required edits across multiple modules, increasing the change surface, maintenance burden, and risk of regression.

2) **Incomplete adherence to the WebAssembly specification**
   - Several required spec concepts were not modeled as first‑class, composable abstractions, leading to partial or ad‑hoc handling of sections and semantics.
   - This design gap hindered conformance, interoperability, and the ability to incorporate new standard features or proposals cleanly.

3) **Insufficiently verified memory safety in the parser**
   - The uwvm1 parser lacked comprehensive, manual verification and systematic pre/post‑condition checks.
   - A notable example was an **i32 pointer underflow** vulnerability stemming from unsound pointer arithmetic and missing bounds checks.
   - Collectively, these issues undermined reliability and made the codebase harder to audit and secure.

### How uwvm2 Addresses These Limitations
1) **Concept‑Oriented Programming (COP) foundation**
   - Core Wasm domain concepts (e.g., Module, Section, Type, Import, Memory, Table, Global, Instruction) are modeled explicitly and composed predictably.
   - New capabilities are introduced primarily by adding or specializing concepts, rather than touching many unrelated parts of the system.

2) **Specification‑conformant design**
   - The parser and internal representations map directly to the Wasm specification, reducing ambiguity and drift.
   - Feature gates and proposal-aligned flows make it straightforward to adopt new standard features while remaining compliant.

3) **Memory safety by construction and annotation**
   - Pointer arithmetic and index computations are guarded by explicit invariants, pre/post‑conditions, and bounds checks.
   - The codebase includes exhaustive memory‑safety annotations covering hot paths, including **SIMD** handling, enabling rigorous review and tooling.
   - By design, uwvm2 eliminates classes of issues such as the i32 pointer underflow that affected uwvm1.

4) **Maintainability and evolution**
   - Clear separation of concerns and modular boundaries lower the cost and risk of change.
   - The COP approach improves readability, testability, and long‑term maintainability for both core logic and extensions.

### Deprecation Policy and Migration Guidance
- uwvm1 is frozen; security patches are not planned. All new work should target uwvm2.
- For teams maintaining uwvm1‑based flows, plan to migrate to uwvm2 to benefit from a concept‑oriented architecture, stronger spec conformance, and hardened memory safety.
- Refer to the existing documentation in this repository (e.g., build guides and feature notes) for environment setup, capabilities, and integration points.

### Closing Note
The rearchitecture from uwvm1 to uwvm2 is intentionally principled: it replaces a brittle, tightly‑coupled parser structure with a COP‑based, spec‑aligned system and robust safety posture. This positions uwvm2 as the long‑term foundation for correctness, extensibility, and security.


