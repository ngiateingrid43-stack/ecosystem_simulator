# 🚀 🔬 Documentation Technique : Projet Écosystème C++

========================================================

## Table des Matières Détaillée

===============================

1.  [Introduction et Philosophie du Projet 🎯]
2.  [Architecture Globale et Organisation 🏗️]
    - 2.1. Les Espaces de Noms (Namespaces)
    - 2.2. Le Cycle de Jeu (Game Loop)
3.  [Composants du Cœur de la Simulation (Core) 🧠]
    - 3.1. `Ecosystem::Core::Ecosystem` (Le Gestionnaire de Monde)
    - 3.2. `Ecosystem::Core::Entity` (La Classe de Base des Organismes)
    - 3.3. **Détail des Comportements de Pilotage (Steering Behaviors) 🧭**
4.  [Structures de Données Fondamentales (`Structs.h`) 📐]
5.  [Les Structures de Données Avancées (Utilitaires) 🔗] - 5.1. `ListeSimple` - 5.2. `ListeDouble` 6. [Gestion de la Fenêtre et du Rendu (`Graphics`) 🖼️]
    **Compilation et Lancement du Projet**

---

## 1\. Introduction et Philosophie du Projet 🎯

===============================================

Ce projet est une **simulation multi-agents** où chaque entité agit de manière autonome en fonction de son environnement et de ses besoins physiologiques (faim, survie, reproduction). Le moteur, écrit en C++17, s'appuie sur la librairie **SDL3** pour le rendu graphique fluide et efficace de milliers de petites formes à l'écran.

L'objectif est d'étudier l'émergence de dynamiques complexes à partir de règles simples : les herbivores cherchent la nourriture et fuient les prédateurs, tandis que les carnivores chassent les herbivores pour survivre. Le comportement qui en résulte n'est jamais scripté, mais découle entièrement des algorithmes de **Pilotage (Steering)** appliqués à chaque cycle de jeu. L'équilibre de la simulation dépend fortement du réglage fin des paramètres d'énergie, de vitesse et de portée de détection des entités.

---

## 2\. Architecture Globale et Organisation 🏗️

==============================================

### 2.1. Les Espaces de Noms (Namespaces)

L'utilisation des **Namespaces** (`Ecosystem::Core` et `Ecosystem::Graphics`) est une pratique essentielle en C++ pour éviter les **conflits de noms** entre des classes ou fonctions de même nom.

- **`Ecosystem::Core`** : Contient toute la **logique métier** (l'Écosystème, les Entités, les `Structs`, les algorithmes de mouvement). Ces composants n'ont _aucune_ connaissance de la manière dont ils sont affichés.
- **`Ecosystem::Graphics`** : Gère l'interface avec la librairie **SDL3** (fenêtre, rendu). Il utilise les structures de `Core` (comme `Vector2D` et `Color`) pour dessiner, mais `Core` est indépendant de `Graphics`. Cette **séparation des préoccupations** garantit la flexibilité et la maintenabilité du code.

### 2.2. Le Cycle de Jeu (Game Loop) 🔄

Le fichier `main.cpp` initialise la classe `GameEngine`, dont la méthode `run()` contient la boucle principale.
Chaque itération de cette boucle exécute les étapes suivantes :

1.  **Calcul du `deltaTime`** : Mesure le temps réel écoulé depuis la dernière frame. Ce paramètre est essentiel pour que la simulation soit **indépendante de la vitesse d'affichage**. Une entité se déplace de la même distance, que le jeu tourne à 30 FPS ou 60 FPS.
2.  **Gestion des Événements** : Traitement des entrées utilisateur (pause, reset, ajout de nourriture).
3.  **Mise à Jour de l'Écosystème** : L'appel `mEcosystem.Update(deltaTime)` déclenche **toute** la logique de la simulation.
4.  **Rendu** : L'appel `mEcosystem.Render(renderer)` dessine l'état actuel du monde à l'écran.

---

## 3\. Composants du Cœur de la Simulation (Core) 🧠

====================================================

### 3.1. `Ecosystem::Core::Ecosystem` (Le Gestionnaire de Monde)

Cette classe est le maître d'œuvre de la simulation, gérant l'état global et les interactions inter-entités.

- **`std::vector<std::unique_ptr<Entity>> mEntities;`** : Le choix d'utiliser `std::unique_ptr` permet de stocker polymorphiquement tous les types d'entités (Herbivore, Carnivore, Plant) dans un seul conteneur tout en assurant que la mémoire est libérée automatiquement lorsque l'entité meurt et est retirée de la liste (`RemoveDeadEntities`).
- **`void HandleEating()`** : Cette fonction implémente la **chaîne alimentaire**. Elle effectue des vérifications de proximité (collisions) entre :
  - Les **Herbivores** et les `mFoodSources` (Plantes statiques).
  - Les **Carnivores** et les **Herbivores** (leurs proies).
    Si la distance est inférieure au seuil d'alimentation (rayon de l'entité), l'énergie est transférée et, si la cible est un organisme, elle est marquée comme morte.
- **`void HandleReproduction()`** : Parcourt toutes les entités et vérifie `entity->CanReproduce()`. Si l'entité a atteint un seuil d'énergie suffisant et que sa population n'est pas limitée, elle génère un nouvel enfant (`entity->Reproduce()`) qui est ajouté à `mEntities`. Cela simule la croissance de la population.
- **`void RemoveDeadEntities()`** : Cette étape de nettoyage, vitale pour la performance, utilise un algorithme de suppression efficace (souvent `std::remove_if` suivi de `erase`) pour retirer toutes les entités où `IsAlive()` retourne `false`.

### 3.2. `Ecosystem::Core::Entity` (La Classe de Base des Organismes) 🌱

Tous les êtres vivants héritent de cette classe. Elle gère leur cycle de vie et leurs propriétés physiques.

- **Variables d'État Cruciales** :
  - `mEnergy` (float) : La ressource de base. Elle est consommée à chaque `Update` et rechargée par `Eat()`.
  - `mMaxAge` (int) : Le seuil de vieillesse.
  - `mVelocity` (Vector2D) : Le vecteur de vitesse actuel.
- **`void ConsumeEnergy(float deltaTime)`** : Cette méthode est appelée à chaque frame. Elle décrémente `mEnergy` en fonction du `deltaTime` et d'une constante de métabolisme (plus une entité est rapide ou grosse, plus elle consomme). C'est la pression de la faim dans le système.
- **`void CheckVitality()`** : La **méthode de la mort**. Elle est déclenchée après la consommation d'énergie et après la vieillesse. Si `mEnergy <= 0.0f` ou si `mAge >= mMaxAge`, l'entité passe à l'état `mIsAlive = false`.
- **`void ApplyForce(Vector2D force)`** : Mécanisme de **physique de base**. La force (`force`) est divisée par la masse de l'entité pour obtenir une accélération. Cette accélération est ensuite ajoutée à la vélocité (`mVelocity`), qui est plafonnée par une `mMaxSpeed`.

### 3.3. **Détail des Comportements de Pilotage (Steering Behaviors) 🧭**

Ces fonctions utilisent le concept de **Force de Pilotage (Steering Force)**, définie comme la différence entre la **Vitesse Désirée** et la **Vitesse Actuelle** de l'entité.

#### 1\. `Vector2D SeekFood(const std::vector<Food>& foodSources) const` 🥕

1.  **Détection de Cible** : Utilise `Vector2D::DistanceSq` pour trouver la source de nourriture la plus proche **sans calculer de racine carrée**, ce qui est une optimisation essentielle pour les boucles de jeu.
2.  **Calcul du Vecteur Désiré** : `Vector2D desired = closestFoodPos - position;`
3.  **Normalisation** : Le vecteur est normalisé (`desired.Normalize()`) puis mis à l'échelle de la vitesse maximale (`mMaxSpeed`).
4.  **Retour** : `return desired - mVelocity;` (Force = Vitesse Désirée - Vitesse Actuelle).

#### 2\. `Vector2D AvoidPredators(const std::vector<const Entity*>& predators) const` 🏃💨

Ce comportement est plus complexe, car il combine plusieurs forces de fuite.

1.  **Rayon de Perception** : L'herbivore utilise un `detectionRadius` (e.g., 150px). Seuls les prédateurs dans ce rayon influencent son mouvement.
2.  **Force de Répulsion** : Pour chaque prédateur à portée, la fonction calcule un vecteur qui **s'éloigne du prédateur**. Ce vecteur est **pondéré** : plus l'entité est proche du prédateur, plus la force de répulsion est grande.
3.  **Combinaison et Pondération** : Dans `Ecosystem::Update`, la force `AvoidPredators` est multipliée par un facteur élevé (e.g., `5.0f`) car la survie doit être prioritaire sur la faim.

<!-- end list -->

```cpp
// Logique de Pondération dans Ecosystem::Update (Explication renforcée)
if (avoidForce.LengthSq() > 0.0f) {
    // 🛑 Urgence : Le facteur 5.0f assure que la force d'évitement est dominante,
    // annulant efficacement toute force de recherche de nourriture simultanée.
    steeringForce = steeringForce + avoidForce * 5.0f;
} else {
    // ✅ Comportement normal : La recherche de nourriture est appliquée avec un poids standard.
    steeringForce = steeringForce + seekForce * 1.0f;
}
```

---

## 4\. Structures de Données Fondamentales (`Structs.h`) 📐

===========================================================

### `struct Vector2D`

La structure de base pour la physique et la géométrie 2D.

- **`float Distance(const Vector2D& other) const`** : Calcule la distance euclidienne standard.
- **`float Length() const`** : Calcule la magnitude du vecteur (`std::sqrt(x*x + y*y)`).
- **`operator+` et `operator*`** : Surcharges d'opérateurs pour des manipulations vectorielles intuitives : `v3 = v1 + v2;` ou `v2 = v1 * 5.0f;`. Ceci est fondamental pour l'accumulation et la mise à l'échelle des forces de pilotage.

### `struct Color`

Utilisée pour définir les couleurs des entités et du rendu SDL. Les fonctions statiques (`Color::Red()`, `Color::Green()`) améliorent la lisibilité par rapport à l'utilisation de valeurs hexadécimales brutes.

---

## 5\. Les Structures de Données Avancées (Utilitaires) 🔗

==========================================================

Les fichiers `ListS.h/cpp` et `ListD.h/cpp` démontrent une maîtrise des structures de données dynamiques sans la STL.

### 5.1. `ListeSimple` (`ListS.h/cpp`)

| Fonction                                  | Algorithme et Complexité    | Explication Détaillée                                                                                                                                                                                                                                                   |
| :---------------------------------------- | :-------------------------- | :---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `NoeudS* TrouverMilieu(...)`              | **Lièvre et Tortue (O(n))** | Utilise deux pointeurs : le **lent** avance d'un nœud à la fois, le **rapide** avance de deux. Lorsque le rapide atteint la fin, le lent est positionné exactement au milieu de la liste. Cela permet de trouver le milieu en un seul passage.                          |
| `bool EstPalindrome(...)`                 | **O(n)**                    | Nécessite d'utiliser `TrouverMilieu`, d'inverser la seconde moitié de la liste, puis de comparer les deux moitiés élément par élément. Cette complexité vient de la nécessité de parcourir la liste, de l'inverser, puis de la parcourir à nouveau pour la comparaison. |
| `ListeSimple* FusionnerListesTriees(...)` | **O(n + m)**                | Fusionne deux listes triées sans utiliser de tri. Elle prend l'élément le plus petit des deux têtes à chaque étape et l'insère dans la nouvelle liste, garantissant que la liste résultante reste triée.                                                                |

### 5.2. `ListeDouble` (`ListD.h/cpp`)

| Fonction                   | Algorithme et Complexité | Explication Détaillée                                                                                                                                                                                                                                                                                    |
| :------------------------- | :----------------------- | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `bool EstPalindrome(...)`  | **O(n)**                 | Plus simple que pour la liste simple : un pointeur part de `tete` et un autre de `queue`. Les deux pointeurs convergent vers le centre en se déplaçant simultanément (`debut->suivant` et `fin->precedent`), permettant de comparer les éléments par paires pour vérifier la propriété de palindrome.    |
| `bool EchangerNoeuds(...)` | **O(1) ou O(n)**         | La complexité est O(n) s'il faut d'abord chercher les nœuds. Si les pointeurs sont fournis, l'échange est O(1). L'algorithme se concentre sur la mise à jour minutieuse des quatre pointeurs `precedent` et `suivant` des nœuds voisins, incluant la gestion des cas aux extrémités (`tete` et `queue`). |

---

## 6\. Gestion de la Fenêtre et du Rendu (`Graphics`) 🖼️

========================================================

La classe `Ecosystem::Graphics::Window` encapsule toutes les fonctions de bas niveau de la SDL pour garantir que le reste du code C++ reste propre et indépendant des bibliothèques graphiques.

- **`bool Initialize()`** : S'occupe d'appeler `SDL_Init(SDL_INIT_VIDEO)`, de créer l'objet `SDL_Window*` et surtout le `SDL_Renderer*`. Le **Renderer** est l'objet clé que toutes les entités utilisent pour dessiner.
- **`void Clear(...)`** : Remplissage de l'écran avec une couleur de fond. C'est le premier appel de rendu dans la boucle de jeu.
- **`void Present()`** : Commande finale qui **échange les buffers** et affiche le dessin de la frame. Sans cet appel (`SDL_RenderPresent`), rien ne s'affiche à l'écran.

Absolument. Voici une section détaillée pour votre `README.md` qui explique la compilation et l'exécution de votre projet, en tenant compte de l'environnement C++17 et de la librairie SDL3.

---

## 🛠️ Compilation et Lancement du Projet 🎮

Cette section détaille les étapes nécessaires pour compiler et exécuter le simulateur d'écosystème. Le projet a été conçu pour être compilé avec les standards modernes de C++ et nécessite des librairies externes.

### 1\. Prérequis Techniques et Dépendances

Votre projet utilise le standard **C++17** et dépend de la librairie graphique **SDL3** (Simple DirectMedia Layer, version 3.0+).

- **Compilateur :** **g++** (version 9 ou supérieure) ou **Clang** (version 10 ou supérieure).
- **Librairie Graphique :** **SDL3**. Elle doit être installée sur votre système et les fichiers d'en-tête (headers) doivent être accessibles par votre compilateur.

### 2\. Procédure de Compilation

Étant donné la structure du projet (fichiers sources dans `src/` et potentiellement des sous-dossiers comme `src/Core/` et `src/Graphics/`), la compilation nécessite de spécifier tous les fichiers sources ainsi que les drapeaux de linkage (liage) de la SDL3.

#### Exemple avec g++ (pour Linux/macOS ou MinGW)

Utilisez la commande suivante pour compiler l'ensemble des fichiers sources en utilisant les drapeaux de compilation standard :

```bash
# 1. Compilation des fichiers source, en spécifiant le standard C++17 (-std=c++17)
# 2. Le drapeau -Iinclude indique au compilateur où trouver vos headers (.hpp)
# 3. Les drapeaux -lSDL3 -lm lient le programme avec la librairie SDL3 et la librairie mathématique
g++ -std=c++17 -Iinclude -o ecosystem \
    src/*.cpp src/Core/*.cpp src/Graphics/*.cpp \
    -lSDL3 -lm
```

> **Note sur le Linkage 🔗 :**
> L'utilisation du drapeau `-lSDL3` est fondamentale. Il indique au linker d'inclure le code binaire de la librairie SDL3 pour que les fonctions comme `SDL_Init()` et `SDL_CreateWindow()` (utilisées dans `Window.cpp` et `main.cpp`) puissent être résolues lors de la création de l'exécutable `ecosystem`.

### 3\. Exécution de la Simulation

Une fois la compilation réussie, vous pouvez lancer la simulation directement :

```bash
./ecosystem
```

Le programme affichera les messages d'initialisation (création de la fenêtre et de l'écosystème) avant de démarrer la boucle de jeu.

### 4\. Commandes de Contrôle en Jeu 🕹️

Les commandes suivantes sont intégrées dans la boucle de jeu et gérées par le `GameEngine` pour interagir avec la simulation :

| Touche      | Action                  | Explication                                                                              |
| :---------- | :---------------------- | :--------------------------------------------------------------------------------------- |
| **ESPACE**  | **Pause/Reprise**       | Arrête le compteur de temps (`deltaTime = 0`) et gèle tous les mouvements et la logique. |
| **R**       | **Réinitialisation**    | Réinitialise l'écosystème à son état initial (populations et placement).                 |
| **F**       | **Ajout de Nourriture** | Appelle `Ecosystem::SpawnFood()` pour ajouter des sources de nourriture statiques.       |
| **FLÈCHES** | **Contrôle de Vitesse** | Peut être utilisé pour accélérer ou ralentir la simulation en manipulant le `deltaTime`. |
| **ÉCHAP**   | **Quitter**             | Ferme la fenêtre et appelle `Window::Shutdown()` pour nettoyer les ressources SDL.       |

---

> 💡 **Citation pour l'Inspiration et la Persévérance :**
>
> **« Le succès n'est pas final, l'échec n'est pas fatal : c'est le courage de continuer qui compte. La science ne décrit pas la nature telle qu'elle est, mais telle qu'elle apparaît au travers des modèles que nous construisons. »** — _Winston Churchill et Carl Sagan (combinés pour le développement scientifique)._
>
> _Le développement logiciel, tout comme la science, est un processus itératif. Chaque bug résolu, chaque fonction implémentée et chaque entité qui se déplace de manière réaliste est une victoire. Continuez à affiner vos modèles et votre courage à déboguer \!_

