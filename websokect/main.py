# -*- coding: utf-8 -*-

import asyncio
import websockets

# Dictionnaire pour mapper les clients � leurs connexions WebSocket
client_connections = {}
print("lancer")
async def handler(websocket, path):
    # Ajouter le client � la liste
    client_id = id(websocket)
    client_connections[client_id] = websocket
    try:
        async for message in websocket:
            # Afficher le message re�u dans la console du serveur
            print(f"Message recu du client {client_id}: {message}")
            # Relayer le message � tous les autres clients
            await relay_message(message, client_id)
    except websockets.exceptions.ConnectionClosedOK:
        pass  # Ignorer les fermetures de connexion normales
    except websockets.exceptions.ConnectionClosedError as e:
        if client_id in client_connections:
            del client_connections[client_id]  # Supprimer le client de la liste s'il est encore pr�sent
        print(f"Erreur de connexion avec le client {client_id}: {e}")
    except Exception as e:
        print(f"Erreur inattendue avec le client {client_id}: {e}")
    finally:
        pass  # Aucune action suppl�mentaire n�cessaire ici

async def relay_message(message, sender_id):
    # Envoyer le message � tous les clients sauf � l'exp�diteur
    for client_id, connection in client_connections.items():
        if client_id != sender_id:
            try:
                await connection.send(message)
            except websockets.exceptions.ConnectionClosedOK:
                pass  # Ignorer les fermetures de connexion normales
            except websockets.exceptions.ConnectionClosedError as e:
                del client_connections[client_id]  # Supprimer le client de la liste s'il est d�connect�
                print(f"Erreur de connexion avec le client {client_id}: {e}")
            except Exception as e:
                print(f"Erreur inattendue avec le client {client_id}: {e}")

# Configuration du serveur WebSocket
start_server = websockets.serve(handler, "0.0.0.0", 81, ping_timeout=None)

# Fonction principale pour ex�cuter le serveur
async def main():
    async with start_server as server:
        await server.serve_forever()

# Ex�cution de la fonction principale avec la boucle d'�v�nements par d�faut
if __name__ == "__main__":
    asyncio.get_event_loop().run_until_complete(main())
