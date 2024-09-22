import {Component, OnInit} from '@angular/core';
import {Subscription} from "rxjs";
import {MenuService} from "../../services/menu.service";
import {ApiService} from "../../services/api.service";

@Component({
  selector: 'app-admin-buttons',
  templateUrl: './admin-buttons.component.html',
  styleUrls: ['./admin-buttons.component.css']
})
export class AdminButtonsComponent implements OnInit{
  subscription: Subscription;
  loginSuccessful: boolean;
  admin: boolean;
  constructor(private menuService: MenuService, private loginService: ApiService) {
    this.subscription = this.menuService.getLoginInfo().subscribe(value => {
      this.loginSuccessful = value;
    })
    this.menuService.adminStatus.subscribe((status) => {
      this.admin = status;
    })
  }

  ngOnInit() {
    this.loginSuccessful = this.menuService.getLoggedIn();
    this.admin = this.menuService.getAdmin();
    this.loginService.inbox();
  }

  inbox(){
    this.loginService.inbox();
  }
  outbox() {
    this.loginService.outbox();
  }

  logout(){
    this.menuService.logout();
    this.loginService.logout().subscribe((response) => {
      console.log(response)
    });
    sessionStorage.setItem('clientToken', "");
  }



}
