/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oroy <oroy@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 17:59:11 by olivierroy        #+#    #+#             */
/*   Updated: 2024/07/10 13:10:31 by oroy             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/WebServer.hpp"
#include "../includes/Socket.hpp"

int	main(int argc, char **argv, char **envp)
{
	Socket				socket0("localhost", 8080);
	Socket				socket1("127.0.0.1", 4242);
	std::vector<Socket>	socketList;

	socketList.push_back(socket0);
	socketList.push_back(socket1);

	WebServer			webServer(socketList);

	(void) argc;
	(void) argv;
	(void) envp;

	if (webServer.init() < 0)
	{	
		return (-1);
	}
	webServer.run();
	return (0);
}
