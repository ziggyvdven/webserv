/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_test.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oroy <oroy@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 17:59:11 by olivierroy        #+#    #+#             */
/*   Updated: 2024/07/04 15:42:33 by oroy             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/WebServer.hpp"

int	main(void)
{
	WebServer	webServer;

	if (webServer.init() != 0)
	{	
		return (1);
	}
	webServer.run();
	return (0);
}
