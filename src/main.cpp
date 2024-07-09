/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oroy <oroy@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 17:59:11 by olivierroy        #+#    #+#             */
/*   Updated: 2024/07/09 18:03:51 by oroy             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/WebServer.hpp"
#include "../includes/Socket.hpp"

int	main(int argc, char **argv, char **envp)
{
	WebServer			webServer;
	Socket				socket0("localhost", 8080);
	Socket				socket1("127.0.0.1", 4242);
	std::vector<Socket>	socketList;

	socketList.push_back(socket0);
	socketList.push_back(socket1);

	// if (argc != 2)
	// {
	// 	std::cerr << "Please enter the configuration file name" << std::endl;
	// 	std::cerr << "└─> ./webserv \"config.conf\"" << std::endl;
	// 	return (1);
	// }
	(void) argc;
	(void) argv;
	(void) envp;

	if (webServer.init(socketList) < 0)
	{	
		return (-1);
	}
	webServer.run();
	return (0);
}
