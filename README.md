# TP Web Service REST — kit de démarrage (C++20, Boost.Beast, Boost.JSON)

## Dépendances

Ubuntu / Debian (Boost ≥ 1.81) :

    sudo apt install build-essential cmake curl libboost-dev libboost-json-dev

## Compilation et exécution

    cmake -S . -B build
    cmake --build build
    ./build/server            # écoute sur http://127.0.0.1:8080
    ./build/server 9000       # autre port

Dans un second terminal :

    curl -i http://localhost:8080/version
    ./test.sh                 # relancer le serveur avant chaque passage

## Contenu

- `src/` : sources du kit ; les fichiers à compléter contiennent des `TODO`
  (`ServicesManager.cpp`, `VersionService.cpp`, `UserService.cpp`, `main.cpp`).
- `dia/classes.dia` : diagramme UML des classes (éditable avec Dia).
- `test.sh` : tests de validation (curl), étiquetés par question.
## Travail réalisé

### Q1 à Q3 — Service de version et routage

Nous avons commencé par traiter directement la requête `/version` dans
`ServicesManager`, puis nous avons déplacé cette logique vers la classe
`VersionService` afin de séparer le routage du traitement des requêtes.

Les services sont enregistrés dans un vecteur de `std::unique_ptr`.
Pour identifier le service demandé, nous comparons son motif avec le
début de l'URL, en vérifiant le séparateur `/` afin d'éviter les
correspondances incorrectes, comme `/versions`.
Les paramètres de requête sont retirés avant cette recherche.

### Q4 à Q6 — Lecture d'un utilisateur (GET)

Nous avons distingué les requêtes visant une collection de celles visant
un utilisateur précis grâce à `std::optional<int>`. La conversion de
l'identifiant avec `std::from_chars()` permet de détecter les identifiants
invalides, notamment ceux qui sont partiellement numériques.

Nous avons centralisé l'aiguillage des méthodes HTTP dans un `switch`
au niveau de `ServicesManager`. Pour GET, nous récupérons l'utilisateur
depuis `UserDB` et utilisons la conversion JSON déjà définie dans `User.h`.

### Q7 et Q8 — Création d'un utilisateur (POST)

Nous avons séparé l'analyse syntaxique du JSON, effectuée dans
`ServicesManager` avec `json::parse()`, de la validation des données,
effectuée dans `UserService`.

L'utilisation de `if_contains()` permet de vérifier la présence des
attributs avant d'accéder à leurs valeurs. L'insertion dans la base
intervient uniquement après la validation complète du nom et de l'âge.
L'identifiant retourné par `addUser()` est ensuite intégré à la réponse
JSON, accompagnée du statut `201 Created`.

### Q9 et Q10 — Modification d'un utilisateur (PATCH)

Nous avons choisi de travailler sur une copie locale de l'utilisateur
plutôt que de modifier directement les données stockées dans la base.

Les attributs reçus sont parcourus individuellement et seules les valeurs
présentes dans la requête sont modifiées. L'appel à `setUser()` intervient
après la validation de tous les champs, ce qui garantit l'atomicité de
la modification en cas d'erreur de validation.

Nous avons également adapté la sérialisation dans `ServicesManager`
pour laisser le corps de la réponse vide lorsque le statut retourné
est `204 No Content`.

### Q11 et Q12 — Suppression d'un utilisateur (DELETE)

Nous avons exploité directement la valeur booléenne retournée par
`UserDB::removeUser()` pour déterminer si la suppression a réussi,
évitant ainsi une recherche préalable de l'utilisateur.

Le traitement de DELETE réutilise le mécanisme de routage et d'extraction
de l'identifiant mis en place précédemment, sans nécessiter d'analyse
ni de sérialisation JSON en cas de succès.

### Q13 — Récupération de la collection d'utilisateurs

Nous avons étendu la méthode `UserService::get()` existante sans créer
de service supplémentaire.

En l'absence d'identifiant, nous parcourons la `std::map` de `UserDB`
à l'aide des structured bindings. La clé de chaque entrée est ajoutée
à la représentation JSON de l'utilisateur, puisque son identifiant
est stocké séparément de ses attributs.

Les objets obtenus sont regroupés dans un `json::array`.
Le comportement initial de GET avec identifiant reste inchangé.