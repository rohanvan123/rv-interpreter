Functions => cannot handle nest return statments
returns need to be an expression based attribute
every expression will have field returnable
Detect in arithmetic parser weather or not it is returnable by first token, then set it to true
for while or if statements, if an expression is returnable, then we stop iterating and return the statment, the loop evaluates to this stament there with that parent expression also being returnable;
