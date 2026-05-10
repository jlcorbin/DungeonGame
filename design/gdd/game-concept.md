# Game Concept: Hub & Hollow *(working title)*

*Created: 2026-05-10*
*Status: Draft — Lean review mode, awaiting downstream validation via `/design-review`*

---

## Elevator Pitch

> **It's a stylized-low-poly ARPG dungeon crawler where you delve a hand-crafted dungeon beneath a small frontier town, fight through snappy combat, and bring loot home to a merchant who pays well — but die before you extract and you lose everything you didn't carry out. A starter game built to grow.**

Test: A new player understands the loop — *go down, fight, find stuff, come back, sell, upgrade, go deeper* — in 10 seconds. ✓

---

## Core Identity

| Aspect | Detail |
| ---- | ---- |
| **Genre** | Action RPG / Dungeon Crawler with light roguelite extraction tension |
| **Platform** | PC (Steam) primary, **mobile pre-designed in parallel** (touch input + mobile-aware perf budgets from day one) |
| **Target Audience** | Explorers + Achievers (Bartle); Discovery + Completion (Quantic Foundry) |
| **Player Count** | Single-player |
| **Session Length** | Variable — 30 min casual / 60-90 min typical / 120 min deep dive |
| **Monetization** | Premium (one-time purchase); no F2P / IAP / battle pass |
| **Estimated Scope** | Small-Medium starter (6-9 months, solo) — designed to be expandable into Medium-Large (12-24 months) in later installments |
| **Comparable Titles** | *Moonlighter* (most direct comparable), *Tunic*, *Death's Door*, *Hollow Knight* (hand-crafted Metroidvania feel) |

---

## Core Fantasy

**You are the treasure-hunter with a home to come back to.**

Two emotional states alternate in clean contrast:
- **In the dungeon** — tense, kinetic, in flow. Every cleared room asks: push deeper or play safe? The dungeon does not care about you.
- **In town** — calm, social, warm. The merchant remembers you. The innkeeper knows your name. Your gold is safe here, and so are you.

The promise is not power-over-everything (this isn't a Souls-like). It is not narrative immersion (this isn't an Assassin's Creed). The promise is *the joy of returning home with something rare in your pack* — and the slow, visible growth of your character and your town as that pack fills, week after week.

---

## Unique Hook

**Like *Moonlighter*, AND ALSO every room is hand-placed, every secret is hand-hidden, and the entire game world fits inside one town + one dungeon — a deliberately small but deeply crafted single-location ARPG, built to be a starter chapter that opens the door to more.**

The hook passes the "and also" test by combining two pieces:
1. **Tight extraction-roguelite tension** (Moonlighter's "extract or push?" loop)
2. **Hand-crafted everything** (no procedural shortcuts, every secret is intentional)
3. **Starter-as-foundation framing** (the game ends with a satisfying boss who's revealed to be a *lieutenant*, opening into expansions without leaving this game feeling unfinished)

The "lieutenant boss → general later" structure is the connective tissue between this starter and any sequel/expansion. It means players get a real ending *and* a real promise.

---

## Player Experience Analysis (MDA Framework)

### Target Aesthetics (What the player FEELS)

| Aesthetic | Priority | How We Deliver It |
| ---- | ---- | ---- |
| **Sensation** (sensory pleasure) | **2** | Snappy combat with animation cancelling, satisfying hit/dodge/loot-drop feedback. Visceral juice on every action. |
| **Fantasy** (make-believe, role-playing) | 5 | Light — classic fantasy iconography (knights, skeletons, treasures) provides a familiar costume but the game doesn't lean on character roleplay |
| **Narrative** (drama, story arc) | 7 | Minimal — light spine via NPC dialogue progression + scattered lore notes. Story is flavor, not driver. |
| **Challenge** (obstacle course, mastery) | 6 | Combat is intentionally forgiving in execution; meta-strategy (extract decisions, gear choices) is where difficulty lives |
| **Fellowship** (social connection) | N/A | Single-player only |
| **Discovery** (exploration, secrets) | **1 — PRIMARY** | Hand-placed secrets, rare item identification, optional rooms, lore notes, dungeon shortcuts. Finding things is the headline. |
| **Expression** (self-expression, creativity) | 8 | Minimal — light gear-loadout choice but no build trees or character customization in the starter |
| **Submission** (relaxation, comfort zone) | **3** | The town hub is the flow-state space. Sell, identify, upgrade — quiet, satisfying, low-stakes loops between tense delves. |

### Key Dynamics (Emergent player behaviors)

- **Players will weigh "push deeper vs extract" decisions in their head**, gradually developing their own risk tolerance
- **Players will optimize their "carry priority"** — which items go in the safe slots, which they're willing to risk on a deeper push
- **Players will learn the dungeon's hand-crafted geography** — discovering shortcuts, remembering which corner held the rare drop last time
- **Players will form attachments to specific town NPCs**, even with minimal dialogue, because they're consistent and they remember player progress
- **Players will return between sessions thinking about what they could buy with banked gold** — the merchant's inventory becomes a wish-list

### Core Mechanics (Systems we build)

1. **Snappy melee ARPG combat** — combo chains, dodge with i-frames, stamina management, hit reactions (most of this is already implemented)
2. **Hand-crafted dungeon with extraction tension** — 3 zones, hybrid extraction (checkpoints + limited emergency scrolls)
3. **Loot economy with rarity tiers** — drops, banking-on-extract, merchant valuation, gold-driven upgrades
4. **Town hub with persistent NPCs** — merchant, innkeeper, save/heal, visible progression markers
5. **Light story progression** — scattered lore notes, NPC dialogue states tied to dungeon milestones

---

## Player Motivation Profile

### Primary Psychological Needs Served

| Need | How This Game Satisfies It | Strength |
| ---- | ---- | ---- |
| **Autonomy** (freedom, meaningful choice) | Player chooses when to extract, what to carry, what gear to buy, what order to explore. Strong agency in the strategic layer. | **Core** |
| **Competence** (mastery, skill growth) | Combat is generous so wins come often; gear, town growth, and dungeon depth provide visible objective progress | **Core** |
| **Relatedness** (connection, belonging) | NPCs in town give warmth and continuity, but this isn't a relationships game | Supporting |

### Player Type Appeal (Bartle Taxonomy)

- [x] **Achievers** (goal completion, collection, progression) — How: gear progression, dungeon depth milestones, completion-style item collection, lieutenant boss defeat
- [x] **Explorers** (discovery, understanding systems, finding secrets) — How: hand-placed hidden rooms, rare item identification, lore notes, dungeon shortcuts, NPC dialogue progression
- [ ] **Socializers** — Not served. Single-player, no relationship systems.
- [ ] **Killers/Competitors** — Not served. No PvP, no leaderboards in the starter.

### Flow State Design

- **Onboarding curve**: First delve is short (~5 min), runs into 2-3 easy rooms, kills one enemy, finds one item, returns to town to sell it. The full loop is demonstrated in the first 10 minutes.
- **Difficulty scaling**: Combat tuning stays forgiving throughout; difficulty growth comes from longer delves (more decision pressure), tougher enemy mixes, and bigger gold-investment stakes — not faster reflexes required.
- **Feedback clarity**: Gold-after-sell is the headline metric. Visible town changes (new NPCs, upgraded buildings) signal long-term progress. Gear visuals on character change with upgrades.
- **Recovery from failure**: Death dumps unbanked items but keeps banked gold + town progress + zone-unlocks + checkpoint reach. Respawn at town inn, 30 seconds of dialogue, ready to delve again.

---

## Core Loop

### Moment-to-Moment (30 seconds)
**Player is engaging an enemy or moving through a room.** Snappy combo attacks, dodge with i-frames, stamina rhythm — combat feels good, executes cleanly, ends decisively. Visually clear: enemy health bars, hit reactions, loot drops. *Must be intrinsically satisfying — combat is the "Battlefield mindless flow" layer.*

### Short-Term (5-15 minutes — "one more room")
**Player has just cleared a room with loot in their pack.** The 5-minute loop is the **extract-or-push decision** that triggers at every room (or every cleared room within a zone). Strategic resources in play:
- Inventory slots (carry capacity limits)
- Emergency scrolls (limited, expensive)
- Checkpoint distance (how far back to safety)
- Estimated remaining stamina/potions
- Estimated room danger ahead

This is where "one more turn" psychology lives. The right answer isn't always "extract" or always "push" — it depends on the run.

### Session-Level (30-120 minutes)
A typical session:
1. **Start in town** (1-2 min): merchant identification & sell, gear upgrade, hear new rumor
2. **Enter dungeon** (10-30 min per delve, variable by zone)
3. **Return to town** (1-2 min): sell, identify, plan next delve
4. Repeat 1-3 times per session
5. **Natural stopping point**: after a successful delve + sell + upgrade. The town visit closes the emotional loop.

**Hook for next session**: "I'm one delve away from Zone 2" / "I have enough gold for the new sword" / "There was a locked door I haven't figured out yet."

### Long-Term Progression
Four axes, ranked by priority:
1. **Dungeon depth** (primary signal of "I'm progressing"): Zone 1 → Zone 2 → Zone 3 → Lieutenant boss
2. **Character power** (small, visible, frequent): gear tiers 1-3, consumables, eventually 1-2 new combat abilities
3. **Town growth** (cosmetic-but-meaningful): NPCs added at milestones, building upgrades, environmental changes
4. **Story revelation** (light): lore notes accumulate into a picture of *what* the lieutenant guards and *who* the general really is

**End-state**: lieutenant defeated → epilogue cutscene/dialogue → "to be continued" hook for expansion.

### Retention Hooks
- **Curiosity**: locked doors, unidentified items, NPC hints at deeper truths, hidden rooms players haven't found yet
- **Investment**: banked gold accumulating toward a specific gear purchase; town visibly grown
- **Social**: N/A (single-player)
- **Mastery**: optimization of carry priority, route knowledge, combat efficiency

---

## Game Pillars

### Pillar 1 — Town is Sanctuary, Dungeon is Stakes
The hub is unambiguously safe, warm, and social. The dungeon is unambiguously tense. The contrast between them is the game's emotional structure — we never blur it.

*Design test*: If we're debating whether to add a combat encounter or threat to town, this pillar says **no**. If we're debating whether to add a fully-safe rest zone in the middle of the dungeon, this pillar says **only at checkpoints, and even then with restrained visual cues — not town-level coziness**.

### Pillar 2 — Snappy Hands, Strategic Mind
Combat itself is fast, generous, and feels good moment-to-moment. The decisions that test the player live one layer up: when to extract, what to identify, what to push for.

*Design test*: If we're debating "add a parry window with strict timing" vs "streamline attack feedback," this pillar says **streamline**. If we're debating "add more enemy types" vs "add more loot variety with identification puzzles," this pillar says **loot variety** — the strategic layer is the puzzle.

### Pillar 3 — Discovery is the Reward
Finding things is why you play. Item hunts, secret rooms, lore notes, rare drops, hidden NPCs in town. Skill mastery is welcome but never headlines.

*Design test*: If we're debating "add a difficulty-modifier system" vs "add 5 more hidden items," this pillar says **hidden items**. If we're debating "boss-rush mode for endgame" vs "secret deep-dungeon NPC who trades lore for rare items," this pillar says **NPC**.

### Pillar 4 — Built to Grow
This is a starter game with a complete loop today, designed to expand cleanly tomorrow. Data-driven where it costs little; hardcoded only where it costs much. Every system that ships should be designed to extend, not box itself in.

*Design test*: If we're debating "hardcoded 3-zone progression" vs "data-driven zone descriptor system," this pillar says **data-driven** — even with a small upfront cost. If we're debating "polish-cut a system entirely" vs "ship a half-implemented version," this pillar says **cut entirely** — half-done systems become debt the expansion has to clean up.

### Anti-Pillars (What This Game Is NOT — permanent)

- **NOT a Souls-like.** Combat stays forgiving in execution; difficulty lives in strategy. We will not add punishing timing windows, brutal damage, or boss-fight gear-checks that demand specific builds.
- **NOT a shop simulator.** The merchant is an NPC, not a minigame. We will not add customer haggling, shop décor, pricing decisions, shopkeeper schedules, or storefront management.
- **NOT an open world.** One town. One dungeon. The world map ends at the dungeon gate. Expansion adds *more dungeons*, never a sprawling overworld.

### Idea Box — Future Expansion Candidates *(deliberately deferred, not anti-pillars)*

These are NOT in scope for this starter, but they are not permanent rejections. Recorded so we don't lose them:
- **Procedural dungeon generation** — possible Endless Mode or expansion-zone power-up
- **Story-driven systems** — branching dialogue, cutscenes, NPC quest chains, character relationships
- **Multiple dungeons / second town / overworld** — natural expansion vector after the starter ships
- **Crafting / smithing as a player-driven system** — currently NPC-purchase only; could become a real subsystem
- **Difficulty modifiers / NG+** — replay layer
- **Co-op or asymmetric multiplayer** — distant future, would be a major reshape

---

## Visual Identity Anchor *(lean-mode placeholder — formalize via `/art-bible`)*

**Direction name**: **Stylized Low-Poly Adventure**

**One-line visual rule**: *Bright readable silhouettes, confident color blocks, chunky proportions — classic fantasy iconography treated with whimsy, not gravitas.*

**Supporting principles**:
1. **Readability over realism.** Every actor, prop, and pickup must read clearly from across a dungeon room. If a silhouette doesn't tell you what something is, it gets simpler. *Design test: when comparing "subtle PBR metal" vs "flat color block with a clear silhouette," choose the silhouette.*
2. **Color confidence.** Saturated, intentional color choices. The dungeon is moody-not-gloomy; the town is warm-not-saccharine. No grayscale-realism, no oppressive desaturation. *Design test: when in doubt, push color saturation up, not down.*
3. **Whimsy in iconography.** Skeletons can look slightly cartoonish; chests can be oversized; coins can sparkle visibly. The world takes itself just-seriously-enough. *Design test: when comparing "menacing realism" vs "charming threat," choose charming threat.*

**Color philosophy**: Town palette leans warm — torch-amber, sunset-rose, cream stone, deep wood. Dungeon palette leans cool — stone-blue, moss-green, torch-amber as the warm accent against cool walls. Loot/treasure colors (gold, magic-blue, rare-purple) pop intentionally against either backdrop.

**Asset foundation**: Existing `Content/RPGTinyHeroWavePBR` asset pack defines the character proportion language (chunky, hero-shaped, stylized). New assets must match this proportion family.

**Touchstone games** (visual reference, not direct copies): *Tunic*, *A Short Hike*, *Death's Door*, *Mario Odyssey* (color confidence), classic D&D illustration (iconography vocabulary).

---

## Inspiration and References

| Reference | What We Take From It | What We Do Differently | Why It Matters |
| ---- | ---- | ---- | ---- |
| **Moonlighter** | Delve + return + sell loop; extraction-or-push tension; light story spine through NPCs | Single dungeon (not multiple); no shop-management minigame; merchant is an NPC not a job | Validates that this exact loop sells (1M+ copies); proves the audience exists |
| **Tunic** | Hand-crafted exploration secrets; map-as-progression feel; "what's behind this?" curiosity | Combat is ARPG, not isometric; no in-game manual conceit; less puzzle-game density | Validates hand-crafted secret design as a primary retention mechanic |
| **Death's Door** | Solo-dev shipped scope; stylized small-world design; snappy combat with personality | Hub-based instead of metroidvania-overworld; longer per-delve commitment | Validates solo-dev achievability for this kind of scope |
| **Hades** | Snappy generous combat; cheap-failure cycle; runs-as-narrative-vehicle | Hand-crafted (not procedural); merchant economy not boon/buff; story is lighter | Validates the snappy-combat-with-cheap-death economy of player attention |
| **Hollow Knight** | Hand-crafted interconnected world; rewarding exploration; satisfying boss fights as goalposts | Single zoom-in dungeon vs sprawling world; less platforming; less narrative weight | Validates that one carefully-crafted world can sustain 15+ hours |
| **Battlefield** *(user reference)* | Mindless-flow combat feel; combat as kinetic catharsis | Genre, scope, single-player, everything else | Anchors that combat should feel **good not demanding**; we are not making Souls combat |
| **Assassin's Creed** *(user reference)* | Light narrative pulling you forward through gameplay | Scope, structure, scale — completely different | Anchors that *some* narrative spine matters; we won't ignore story entirely |
| **Ark: Survival Evolved** *(user reference)* | Long-term engagement through systems, not story | Single-player not multiplayer; structured not sandbox | Anchors that replayability matters; informs Built-to-Grow pillar |

**Non-game inspirations**: Classic D&D illustration (iconography vocabulary); cozy-hub games like *Stardew Valley* (sanctuary-feel calibration); fantasy short fiction with episodic structure (lieutenant→general framing comes from this tradition).

---

## Target Player Profile

| Attribute | Detail |
| ---- | ---- |
| **Age range** | 25-45 — adults with limited free time, gaming experience, and disposable income for premium indies |
| **Gaming experience** | Mid-core — comfortable with ARPG controls, dodge timing, inventory management. Not necessarily hardcore. |
| **Time availability** | 30-90 minute sessions on weeknights; longer weekend sessions possible. Game must respect their time. |
| **Platform preference** | PC primary (Steam); mobile-friendly secondary for on-the-go play |
| **Current games they play** | *Moonlighter*, *Hades*, *Stardew Valley*, *Hollow Knight*, *Death's Door*, *Loop Hero* |
| **What they're looking for** | A premium, single-player, depth-without-cruelty experience. Discovery-driven. Won't punish them for putting it down for two weeks. |
| **What would turn them away** | Souls-like punishment, gacha mechanics, multiplayer-required content, hours-long unbroken sessions required, narrative-heavy pacing they have to follow |

---

## Technical Considerations

| Consideration | Assessment |
| ---- | ---- |
| **Recommended Engine** | **Unreal Engine 5.7** — already pinned; GAS, Enhanced Input, UMG already configured |
| **Key Technical Challenges** | (1) Mobile + PC dual-target adds significant input/UI/perf work; (2) Touch input for snappy ARPG combat is non-trivial; (3) AI for first real enemy (boss-grade) is unbuilt; (4) Loot identification + economy is unbuilt |
| **Art Style** | 3D stylized low-poly — see Visual Identity Anchor section |
| **Art Pipeline Complexity** | Low-Medium — anchored by existing `RPGTinyHeroWavePBR` and `CharacterCreator` asset packs; supplemental assets needed for town buildings, dungeon kits, item icons, VFX |
| **Audio Needs** | Moderate — footsteps, sword swings, hit sounds, ambient dungeon loop, town music, NPC stinger sounds. Currently unbuilt. |
| **Networking** | **None** — single-player only |
| **Content Volume** | Starter target: 1 town with 2-3 NPCs, 1 dungeon with 3 zones (~25-35 rooms total), 3 enemy archetypes + 1 lieutenant boss, ~15-25 unique items, ~5 lore notes, 3 NPC dialogue states. ~3-5 hours main loop, 15+ hours total with replay. |
| **Procedural Systems** | **None** — explicit anti-pillar. All rooms, encounters, item placements are hand-authored. Variability comes from loot RNG only. |

---

## Risks and Open Questions

### Design Risks
- **R1 — Mobile + PC dual-target compromises both.** Designing for touch from day one risks PC feeling like a "scaled-up mobile game" (oversized UI, simplified controls). Conversely, designing for PC first risks mobile feeling like an afterthought.
  - *Mitigation*: Define platform-native UI scale modes from day one. Touch-controls must feel intentional (e.g., context-sensitive action button) rather than mapping every KB+M binding to an on-screen button.
- **R2 — Harsh death stakes conflict with Relaxation & Flow.** "Lose all unbanked items on death" creates anxiety. The relaxation lives in town, not the dungeon — but if death feels too punishing, players may bounce.
  - *Mitigation*: Tune for "harsh-but-fair" — death should sting in the moment, not undo a week of progress. Banking gold at extract should always preserve meta-progress. Failure should be recoverable in one or two more delves.
- **R3 — Snappy-combat tuning is opposite of current code's direction.** The combat system currently has Souls-y bones (i-frames, stamina, combo windows). Re-tuning to "snappy & forgiving" is a balance pass, not a rewrite, but it requires sustained attention.
  - *Mitigation*: Define combat-feel targets explicitly (e.g., "Player should win a 1v1 against a basic enemy with 0-1 chip damage taken on average competent play"); playtest against the target.

### Technical Risks
- **R4 — First real enemy AI is unbuilt.** Current "enemy" is `DungeonTargetDummy` — unmoving, ungifted target. A real enemy needs `AIController` + perception + behavior tree + animation state machine + GAS integration.
  - *Mitigation*: Treat first enemy as a sprint-sized milestone. Use one templated archetype to validate the AI pipeline before authoring 3 variants.
- **R5 — Loot identification + economy is unbuilt.** Item drops, inventory, rarity tiers, merchant valuation, gold currency, UI for all of the above.
  - *Mitigation*: Build minimum viable economy first (flat gold values, no identification puzzle); promote identification puzzle to Target tier, not MVP.
- **R6 — Touch input for snappy ARPG is unproven.** Combo + dodge + stamina + camera on a phone screen requires careful UX work. No existing reference in your code.
  - *Mitigation*: Prototype touch controls early; treat as a research spike before committing to PC+Mobile dual-target.

### Market Risks
- **R7 — Genre is moderately saturated.** Moonlighter, Hades, Death's Door, Tunic — quality competitors exist. Differentiation rests on the hand-crafted-single-location commitment and the starter-as-foundation framing.
  - *Mitigation*: Lean hard into the unique hook. Marketing must emphasize "intentionally small, intentionally crafted" — make it a feature, not a constraint.
- **R8 — Mobile audience for premium ARPGs is challenging.** Mobile players overwhelmingly prefer F2P. A premium mobile ARPG faces an uphill battle on store discoverability and price-point acceptance.
  - *Mitigation*: Plan for PC primary commercial outcome; mobile is a secondary distribution channel, not a primary revenue source.

### Scope Risks
- **R9 — Solo first-game in 6-9 months for this scope is ambitious.** Even a tight MVP needs combat re-tuning, AI from scratch, loot system, town hub, save/load, full UI, basic audio. That's a lot for one person.
  - *Mitigation*: MVP discipline must be ruthless. Aspirational features get cut without ceremony if the timeline slips. Polish in the final 6 weeks, not the final 6 days.
- **R10 — Mobile dual-target essentially doubles UI and input work.** This is the largest single scope risk.
  - *Mitigation*: Honest conversation at the 50% milestone — if PC starter is at risk, mobile gets deferred to post-launch port.

### Open Questions
- **Q1 — Exact extraction mechanics**: hybrid (checkpoints + scrolls) is locked, but: how many scrolls per delve? Are scrolls purchased, found, or both? Does extracting mid-delve preserve dungeon state (return to same room) or reset zone progress? *Answer via prototype in /map-systems phase.*
- **Q2 — Save model**: autosave-only? Save points only at innkeeper? Save-on-extract? *Answer in technical architecture phase.*
- **Q3 — Identification puzzle design**: if identification ships in Target tier, what does it actually look like? Mini-puzzle? Trial-use? Lore-matching? *Answer via design spec in /design-system for the loot system.*
- **Q4 — Touch input UX**: virtual stick + buttons? Context-sensitive single action? Tap-to-move? *Answer via prototype before mobile commitment is final.*

---

## MVP Definition

**Core hypothesis**: *Players will engage with a hand-crafted ARPG dungeon + town hub loop for 30+ minute sessions, returning across multiple sessions to push deeper, because the extract-or-push decision creates compelling tension and the town hub provides satisfying release.*

**Required for MVP** (must ship — defines "is the loop fun?"):
1. **1 town hub** with merchant (buy/sell at flat gold values), innkeeper (save/heal), 2-3 NPCs total
2. **1 dungeon, 3 zones** (~25-35 hand-crafted rooms), 3 checkpoints, lieutenant boss at the bottom
3. **Combat re-tuned for snappy/forgiving feel** — existing combo + dodge + stamina + hit-react, properly tuned
4. **3 enemy archetypes + 1 lieutenant boss** with AI and animations
5. **Loot system** — drops, inventory, 3 rarity tiers, sold at flat gold values
6. **Extraction mechanic** — checkpoints + 1 emergency scroll per delve (purchased from merchant)
7. **Progression** — gold → buy gear (3 tiers) + consumables
8. **Light story spine** — ~5 lore notes, 3 NPC dialogue states tied to dungeon milestones
9. **Save/load** — innkeeper or autosave on extract
10. **Main menu / pause / options** — basic but complete
11. **Touch input baseline** — virtual stick + 3 on-screen buttons, mobile-aware UI scaling

**Explicitly NOT in MVP** (defer to Target or Aspirational):
- Item identification puzzle (flat gold values only)
- Named unique items
- 5th-6th enemy archetypes
- Additional combat abilities (heavy attack, spells, ranged)
- Town building / NPC adds beyond 2-3
- Cosmetic town growth
- Audio polish beyond minimum SFX
- VFX polish
- NG+ / difficulty modifiers
- Secret-room NPC trades

### Scope Tiers

| Tier | Content | Features | Timeline (solo) |
| ---- | ---- | ---- | ---- |
| **MVP** | 1 town, 3-NPC hub; 1 dungeon, 3 zones, ~25-35 rooms; 3 enemies + 1 boss; 15 items, 3 rarity tiers | Core combat (re-tuned); extraction (checkpoint + scroll); loot economy at flat values; basic save/load; basic menus; touch input baseline | **4-6 months** |
| **Target** | MVP + identification puzzle; 5 enemies + 1 boss + 2 mid-bosses; 25 items + 5 named uniques; 4 rarity tiers; 2 added abilities; town NPC additions; basic audio + VFX pass | + identification system + 2 abilities + audio/VFX | **6-9 months** |
| **Full Vision** | Target + cosmetic town growth + secret NPC + difficulty modifier post-game | + polish layer + NG+ | **9-12 months** |
| **Aspirational** | Full vision + named uniques with special effects + Endless Mode procedural variant | Endless Mode (intentionally NOT a pillar — would be a stretch experiment) | **12+ months** |

---

## Next Steps

In professional pre-production order:
- [ ] Run `/setup-engine` to verify UE 5.7 reference docs are populated and version-pinned (engine is already configured; this validates reference completeness)
- [ ] Run `/art-bible` to formalize the Visual Identity Anchor section into a complete art bible *before* writing GDDs (it gates asset production and shapes technical choices)
- [ ] Run `/design-review design/gdd/game-concept.md` to validate this concept for completeness, consistency, and implementability
- [ ] Optionally discuss vision with the `creative-director` agent for pillar refinement (lean review mode skipped this gate)
- [ ] Run `/map-systems` to decompose the concept into individual systems with dependencies and priorities — produces `design/gdd/systems-index.md`
- [ ] Run `/design-system [system-name]` per identified system to write detailed GDDs (combat, loot, dungeon, town hub, NPCs, extraction, etc.) — note that several of these can be **reverse-documented** from existing code
- [ ] Run `/create-architecture` to produce the master architecture blueprint and Required ADR list (your existing CLAUDE.md gives a head start)
- [ ] Run `/architecture-decision` for each item on the Required ADR list — including retrospective ADRs for already-shipped decisions (IncomingDamage convention, ASC-on-Character, AttackWindow pattern, mobile-first UI choices)
- [ ] Run `/gate-check pre-production` to validate readiness before committing to production sprints
- [ ] Run `/prototype [touch-input]` early — touch-input UX for snappy combat is the single highest-risk technical unknown for the mobile target
- [ ] Run `/playtest-report` after the touch-input prototype to validate the core hypothesis on the most uncertain platform
- [ ] Plan the first production sprint with `/sprint-plan new` once architecture is set

---

*Verdict: **COMPLETE** — game concept created. Concept document handed off to downstream pre-production pipeline.*
